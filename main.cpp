#include <SFML/Graphics.hpp>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <iostream>

using namespace std;

#define random(A,B) (A + (rand() % (int)(B-A+1)))

inline bool es_pared(int tipo, bool bloque_paso) {
    if (tipo == 11 || tipo == 19 || tipo == 27 || tipo == 28 || tipo == 29 || (!bloque_paso && tipo == 18)) {
        return false;
    }
    return true;
}

class Dibujable {
protected:
    sf::Texture* texture_pt;
    sf::Sprite sprite;

    sf::RenderWindow* window;

    // Tamaño del mapa
    int tam_x;
    int tam_y;

    // Posicion en el mapa
    int mapa_x;
    int mapa_y;

    // Posicion de dibujado
    float draw_x;
    float draw_y;
    float draw_xoff;
    float draw_yoff;

    int sprite_id;
    int sprite_animstate;

    int sprite_columns;

    int direction;
    float speed;

    int move_queue;

    int anim_current;
    int anim_movement[4];
    int anim_frames;

    bool moving;
    float move_drawoffx;
    float move_drawoffy;

    bool move_considerpassblock;

    int anim_speed;

    virtual void reach_block() {};
    virtual void ghost_corner() {};

public:

	vector<int>* mapa_matriz;

    //Constructor
    Dibujable(sf::RenderWindow* window_param, sf::Texture* texture_param, vector<int>* mapa_matriz_param) {
        window = window_param;
        texture_pt = texture_param;

        mapa_matriz = mapa_matriz_param;

        sprite.setTexture(*texture_pt);

        move_drawoffx = 0;
        move_drawoffy = 0;

        sprite_animstate = 0;
        sprite_columns = 14;
        anim_current = 0;
        anim_speed = 3;

        sprite.setScale(2, 2);
        sprite.setOrigin(sf::Vector2f(8, 8));

        move_queue = 0;

        move_considerpassblock = true;
    }

    // Necesario para utilizar punteros a objetos heredados de esta clase
    virtual ~Dibujable() {}

    void set_mapsize(int xsize, int ysize) {
		tam_x = xsize;
		tam_y = ysize;
    }

    void dibujar() {

        if (sprite_animstate++ == anim_speed) {
            sprite_animstate = 0;

            if (moving) {
                sprite_id = anim_movement[anim_current];
                anim_current++;

                if (anim_current == anim_frames) {
                    anim_current = 0;
                }
            }
        }

		sprite.setTextureRect(sf::IntRect((sprite_id % sprite_columns) * 16, floor(sprite_id / sprite_columns) * 16, 16, 16));

    	draw_x = mapa_x * 32 + 16 + move_drawoffx + draw_xoff;
    	draw_y = mapa_y * 32 + 16 + move_drawoffy + draw_yoff;

    	sprite.setPosition(sf::Vector2f(draw_x, draw_y));
		window->draw(sprite);
    }

    void dib_mover() {
        if (moving) {
            int draw_pos_x = draw_x - draw_xoff;
            int draw_pos_y = draw_y - draw_yoff;

            bool rot = false;

            switch(direction) {
            case 0:
                if (!es_pared((*mapa_matriz)[(mapa_x + 1) + mapa_y * tam_x], move_considerpassblock)) {
                    move_drawoffx += speed;
                    if (draw_pos_x >= ((mapa_x + 1) * 32) + 16) {
                        move_drawoffx = 0;
                        mapa_x++;

                        rot = true;
                    }
                } else {
                    moving = false;
                    ghost_corner();
                }
                break;

            case 1:
                if (!es_pared((*mapa_matriz)[mapa_x + (mapa_y + 1) * tam_x], move_considerpassblock)) {
                    move_drawoffy += speed;
                    if (draw_pos_y >= ((mapa_y + 1) * 32) + 16) {
                        move_drawoffy = 0;
                        mapa_y++;

                        rot = true;
                    }
                } else {
                    moving = false;
                    ghost_corner();
                }
                break;

            case 2:
                if (!es_pared((*mapa_matriz)[(mapa_x - 1) + mapa_y * tam_x], move_considerpassblock)) {
                    move_drawoffx -= speed;
                    if (draw_pos_x <= ((mapa_x - 1) * 32) + 16) {
                        move_drawoffx = 0;
                        mapa_x--;

                        rot = true;
                    }
                } else {
                    moving = false;
                    ghost_corner();
                }
                break;

            case 3:
                if (!es_pared((*mapa_matriz)[mapa_x + (mapa_y - 1) * tam_x], move_considerpassblock)) {
                    move_drawoffy -= speed;
                    if (draw_pos_y <= ((mapa_y - 1) * 32) + 16) {
                        move_drawoffy = 0;
                        mapa_y--;

                        rot = true;
                    }
                } else {
                    moving = false;
                    ghost_corner();
                }
                break;
            }

            if (rot) {
                switch(move_queue) {
                    case 0: if (!es_pared((*mapa_matriz)[(mapa_x + 1) + mapa_y * tam_x], move_considerpassblock)) direction = move_queue;
                        break;

                    case 1: if (!es_pared((*mapa_matriz)[mapa_x + (mapa_y + 1) * tam_x], move_considerpassblock)) direction = move_queue;
                        break;

                    case 2: if (!es_pared((*mapa_matriz)[(mapa_x - 1) + mapa_y * tam_x], move_considerpassblock)) direction = move_queue;
                        break;

                    case 3: if (!es_pared((*mapa_matriz)[mapa_x + (mapa_y - 1) * tam_x], move_considerpassblock)) direction = move_queue;
                        break;
                }
                reach_block();
            }
        }
    }

    void set_drawoffset(int xoff_param, int yoff_param) {
        draw_xoff = xoff_param;
        draw_yoff = yoff_param;
    }

    void setposition(int x, int y) {
    	move_drawoffx = 0;
		move_drawoffy = 0;

        mapa_x = x;
        mapa_y = y;
    }

    int getposition_x() { return mapa_x; }
    int getposition_y() { return mapa_y; }

    int get_movedrawxoff() { return move_drawoffx; }
    int get_movedrawyoff() { return move_drawoffy; }

    int get_direction() { return direction; }

    void start() { moving = true; }
    void pause() { moving = false; }
};

class Pacman: public Dibujable {

    void reach_block() {
    	switch((*mapa_matriz)[mapa_x + mapa_y * tam_x]) {
		case 29: // Bolita comun
			(*mapa_matriz)[mapa_x + mapa_y * tam_x] = 11;
			score_player += 100;
			break;

		case 27: // Bolitas especiales
		case 28:
			(*mapa_matriz)[mapa_x + mapa_y * tam_x] = 11;
			score_player += 400;
			ate_special = true;
			break;
    	}
    }

public:

    // Puntaje
    int score_player;

    // Comio bolita especial
    bool ate_special;

    // Muerto
    bool dead;

    Pacman(sf::RenderWindow* window_param, sf::Texture* texture_param, vector<int>* mapa_matriz_param) : Dibujable(window_param, texture_param, mapa_matriz_param) {

    	score_player = 0;

    	dead = false;

        sprite_id = 2;

        ate_special = false;

        mapa_x = 1;
        mapa_y = 1;

        direction = 0;
        speed = 1.7;

        anim_frames = 4;

        anim_movement[0] = 0;
        anim_movement[1] = 1;
        anim_movement[2] = 2;
        anim_movement[3] = 1;

        moving = false;
    }

    void reset() {
		score_player = 0;

		sprite_id = 2;
		direction = 0;
		move_queue = 0;
		speed = 1.7;

		moving = false;
    }

    void mover() {
        dib_mover();
        sprite.setRotation(direction * 90);
    }

    void rotar(int dir) {
    	if (dead) return;

        move_queue = dir;

        if (!moving) {
            direction = dir;
            moving = true;
        }
    }

    void death_start() {
		dead = true;
		sprite_id = 3;
		sprite.setRotation(0);
    }

    bool process_death() {
    	sprite_id++;
		if (sprite_id == 13) {
			dead = false;
			return true;
		}

		return false;
    }
};

class Fantasmita : public Dibujable {

	int ghost_spriteid;

    int blocks_passed;

    bool corner;

    bool slowed;
    int slowed_ticks;

    int active_ticks;
    int activation_ticks;
    bool past_firstblock;

    void update_dir_anim() {
    	if (slowed) {
			if (slowed_ticks > 200) {
				anim_movement[0] = 8;
				anim_movement[1] = 9;
			} else {
				anim_movement[0] = 10;
				anim_movement[1] = 9;
			}
    	} else {
			switch(direction) {
			case 0: // Derecha
				anim_movement[0] = ghost_spriteid;
				anim_movement[1] = ghost_spriteid + 1;
				break;

			case 1: // Abajo
				anim_movement[0] = ghost_spriteid + 6;
				anim_movement[1] = ghost_spriteid + 7;
				break;

			case 2: // Izquierda
				anim_movement[0] = ghost_spriteid + 2;
				anim_movement[1] = ghost_spriteid + 3;
				break;

			case 3: // Arriba
				anim_movement[0] = ghost_spriteid + 4;
				anim_movement[1] = ghost_spriteid + 5;
				break;
			}
    	}
    }

    void move_dir_random(int behind) {

    	vector<int> dirs;

		for (int i = 0; i < 4; i++) {
			if (i != behind) {
				int xx = 0, yy = 0;

				switch(i) {
					case 0: xx = 1; break;
					case 1: yy = 1; break;
					case 2: xx = -1; break;
					case 3: yy = -1; break;
				}
				if (!es_pared((*mapa_matriz)[(mapa_x + xx) + (mapa_y + yy) * tam_x], past_firstblock)) {
					dirs.push_back(i);
				}
			}
		}

		if (dirs.size() > 0) {
			direction = move_queue = dirs[random(0, dirs.size() - 1)];
		} else {
			direction = move_queue = behind;
		}
		moving = true;
		update_dir_anim();
    }

    void ghost_corner() {
    	corner = true;
		reach_block();
    }

    void reach_block() {
		// Pasando bloque de activacion
		if ((*mapa_matriz)[mapa_x + mapa_y * tam_x] == 18 && past_firstblock) {
			move_considerpassblock = true;
			return;
		}

    	// Activacion del fantasma
    	if (active_ticks >= activation_ticks && !past_firstblock) {
			// Bloque de paso a la derecha
			if ((*mapa_matriz)[(mapa_x + 1) + mapa_y * tam_x] == 18) {
				direction = 0;
				past_firstblock = true;
				move_considerpassblock = false;
				return;
			}
			// Bloque de paso a la izquierda
			if ((*mapa_matriz)[(mapa_x - 1) + mapa_y * tam_x] == 18) {
				direction = 2;
				past_firstblock = true;
				move_considerpassblock = false;
				return;
			}
			// Bloque de paso arriba
			if ((*mapa_matriz)[mapa_x + (mapa_y - 1) * tam_x] == 18) {
				direction = 3;
				past_firstblock = true;
				move_considerpassblock = false;
				return;
			}
			// Bloque de paso abajo
			if ((*mapa_matriz)[mapa_x + (mapa_y + 1) * tam_x] == 18) {
				direction = 1;
				past_firstblock = true;
				move_considerpassblock = false;
				return;
			}
    	}

    	blocks_passed++;

    	// Si existe la posibilidad de cambiar de eje de movimiento, aumentar la chance
		switch(direction) {
		case 0: // Horizontal
		case 2:
			if (!es_pared((*mapa_matriz)[mapa_x + (mapa_y + 1) * tam_x], past_firstblock) || !es_pared((*mapa_matriz)[mapa_x + (mapa_y - 1) * tam_x], past_firstblock)) {
				blocks_passed += 6;
			}
			break;

		case 1: // Vertical
		case 3:
			if (!es_pared((*mapa_matriz)[(mapa_x + 1) + mapa_y * tam_x], past_firstblock) || !es_pared((*mapa_matriz)[(mapa_x - 1) + mapa_y * tam_x], past_firstblock)) {
				blocks_passed += 6;
			}
			break;
		}

    	if ((blocks_passed > 3 && !random(0, max(10 - blocks_passed, 1))) || corner) {
			int behind_dir = 0;
			blocks_passed = 0;
			corner = false;

			switch(direction) {
			case 0: // Derecha
				behind_dir = 2;
				break;

			case 1: // Abajo
				behind_dir = 3;
				break;

			case 2: // Izquierda
				behind_dir = 0;
				break;

			case 3: // Arriba
				behind_dir = 1;
				break;
			}

			move_dir_random(behind_dir);
    	}
    }

public:
    // Constructor
    Fantasmita(sf::RenderWindow* window_param, sf::Texture* texture_param, vector<int>* mapa_matriz_param) : Dibujable(window_param, texture_param, mapa_matriz_param) {

		blocks_passed = 0;

        anim_frames = 2;
        anim_speed = 10;

        sprite_id = 0;
        ghost_spriteid = 0;

        sprite_columns = 12;

        anim_movement[0] = 0;
        anim_movement[1] = 1;

        speed = 1.5;
        direction = 0;
        moving = false;

        corner = false;

        slowed = false;
        slowed_ticks = 0;

        activation_ticks = 60;
        active_ticks = 0;
        past_firstblock = false;
    }

    void set_activation_ticks(int ticks) {
		activation_ticks = ticks;
    }

    void set_ghost_spriteid(int id) {
		ghost_spriteid = id;
		sprite_id = id;
		update_dir_anim();
    }

    bool is_slowed() { return slowed; }

    void toggle_slowmode() {
		if (!slowed) speed -= 0.5;

		slowed = true;
		slowed_ticks = 600;

		update_dir_anim();
    }

    void process_slowmode() {
		if (slowed) {
			if (slowed_ticks > 0) {
				slowed_ticks--;
			} else {
				slowed = false;
				speed += 0.5;
			}
			update_dir_anim();
		}
    }

    void process_activation() {
    	if (activation_ticks == 0) {
			past_firstblock = true;
            move_considerpassblock = true;
    	}
    	else if (active_ticks < activation_ticks) {
			active_ticks++;
		}
    }
};

class Mapa {
	int tam_x;
	int tam_y;

	int draw_xoff;
	int draw_yoff;

	sf::Texture textura_mapa;
	sf::Sprite sprite_paredes;

	sf::RenderWindow* window;

	vector<int> bol_especiales;
	int bol_anim;

	bool level_loaded;

	// Dibujar una sola pared
	void dibujar_pared(int x, int y, int tipo) {
		sprite_paredes.setTextureRect(sf::IntRect(32 * (tipo % 10), 32 * floor(tipo / 10), 32, 32));

		sprite_paredes.setPosition(sf::Vector2f(x * 32 + draw_xoff, y * 32 + draw_yoff));

		window->draw(sprite_paredes);
	}

	// Crear fantasma en posicion x, y
	// 0 -> Rojo / 1 -> Rosa / 2 -> Azul / 3 -> Naranja
	void createGhost(vector<Fantasmita>& ghosts, sf::Texture* ghost_texture, int tipo, int x, int y) {
		Fantasmita ghost_temp(window, ghost_texture, &mapa_pos);

		ghost_temp.set_ghost_spriteid(tipo * 12);

		ghost_temp.set_activation_ticks(60 * 5 * tipo);

		ghost_temp.setposition(x, y);

		ghost_temp.set_mapsize(tam_x, tam_y);

		ghosts.push_back(ghost_temp);
	}

public:

    vector<int> mapa_pos;

    int pac_spawn_x;
    int pac_spawn_y;

	// Constructor
	Mapa(sf::RenderWindow* window_param) {

		window = window_param;

		textura_mapa.loadFromFile("assets/walls.png");
		sprite_paredes.setTexture(textura_mapa);

		pac_spawn_x = 1;
		pac_spawn_y = 1;

		tam_x = 0;
		tam_y = 0;

		bol_anim = 0;

		level_loaded = false;
	}

	int get_tam_x() { return tam_x; }
	int get_tam_y() { return tam_y; }

	// Cargar nivel
	void load_level(string lvlname) {
		fstream mapfile;
		mapfile.open("mapas/" + lvlname + ".txt");

		if (mapfile.is_open()) {
            string sub_line;

            if (level_loaded) {
				mapa_pos.clear();
            }

            string tam_x_str, tam_y_str;
            getline(mapfile, tam_x_str);
            getline(mapfile, tam_y_str);

            tam_x = atoi(tam_x_str.c_str());
            tam_y = atoi(tam_y_str.c_str());

			int i = 0, j = 0;
            while(getline(mapfile, sub_line, ',')) {
                int pos = atoi(sub_line.c_str());

                switch(pos) {
				case 27: // Bolitas especiales
				case 28:
					mapa_pos.push_back(pos);
					bol_especiales.push_back(i + j * tam_x);
					break;

				case 30: // Posicion inicial pacman
					pac_spawn_x = i;
                    pac_spawn_y = j;
                    mapa_pos.push_back(11);
					break;

				default:
					mapa_pos.push_back(pos);
					break;
                }

                i++;
                if(i == tam_x) {
                    i = 0;
                    j++;
                }
            }

            level_loaded = true;

            mapfile.close();
		}
	}

	// Cargar fantasmas en el nivel
	void load_ghosts(vector<Fantasmita>& ghosts, sf::Texture* ghost_texture) {
		for (int i = 0; i < tam_x; i++) {
			for (int j = 0; j < tam_y; j++) {
				switch(mapa_pos[i + j * tam_x]) {
				case 31: // Rojo
					createGhost(ghosts, ghost_texture, 0, i, j);
					mapa_pos[i + j * tam_x] = 11;
					break;

				case 32: // Rosa
					createGhost(ghosts, ghost_texture, 1, i, j);
					mapa_pos[i + j * tam_x] = 11;
					break;

				case 33: // Azul
					createGhost(ghosts, ghost_texture, 2, i, j);
					mapa_pos[i + j * tam_x] = 11;
					break;

				case 34: // Naranja
					createGhost(ghosts, ghost_texture, 3, i, j);
					mapa_pos[i + j * tam_x] = 11;
					break;
				}
			}
		}
	}

	// Cambiar posicion de dibujado del mapa
	void set_drawoffset(int xoff_param, int yoff_param) {
		draw_xoff = xoff_param;
		draw_yoff = yoff_param;
	}

	// Dibujar el mapa
	void dibujar_mapa() {
		if (!level_loaded) return;

		for (int i = 0; i < tam_x; i++) {
			for (int j = 0; j < tam_y; j++) {
				dibujar_pared(i, j, mapa_pos[i + j * tam_x]);
			}
		}

		if (bol_anim++ == 3) {
			bol_anim = 0;

			for(unsigned int i = 0; i < bol_especiales.size(); i++) {
				int pos = mapa_pos[bol_especiales[i] % tam_x + (bol_especiales[i] - bol_especiales[i] % tam_x)];

				if (pos == 27) {
					mapa_pos[bol_especiales[i] % tam_x + (bol_especiales[i] - bol_especiales[i] % tam_x)] = 28;
				} else if (pos == 28) {
					mapa_pos[bol_especiales[i] % tam_x + (bol_especiales[i] - bol_especiales[i] % tam_x)] = 27;
				}
			}
		}
	}
};

/** FUNCIONES **/

void setDrawOffset(Pacman* pacman, vector<Fantasmita>& ghosts, Mapa* mapa, int xoff, int yoff) {
    pacman->set_drawoffset(xoff, yoff);
    mapa->set_drawoffset(xoff, yoff);

    for (unsigned int i = 0; i < ghosts.size(); i++) {
		ghosts[i].set_drawoffset(xoff, yoff);
    }
};

void pauseAll(vector<Fantasmita>& ghosts, Pacman* player) {
	for (unsigned int i = 0; i < ghosts.size(); i++) {
		ghosts[i].pause();
	}
	player->pause();
}

void checkCollision(Pacman* player, vector<Fantasmita>& ghosts) {
	bool dead = false;

	for (unsigned int i = 0; i < ghosts.size(); i++) {

		if (ghosts[i].is_slowed()) continue;

		if (ghosts[i].getposition_x() == player->getposition_x() && ghosts[i].getposition_y() == player->getposition_y()) {
			dead = true;
		}

		switch(ghosts[i].get_direction()) {
		case 0: // Fantasma derecha
			if (player->getposition_x() - 1 == ghosts[i].getposition_x() && player->getposition_y() == ghosts[i].getposition_y()) {
				if (abs(abs(player->get_movedrawxoff()) - abs(ghosts[i].get_movedrawxoff())) >= 10) {
					dead = true;
				}
			}
			break;

		case 1: // Fantasma abajo
			if (player->getposition_x() == ghosts[i].getposition_x() && player->getposition_y() - 1 == ghosts[i].getposition_y()) {
				if (abs(abs(player->get_movedrawyoff()) - abs(ghosts[i].get_movedrawyoff())) >= 10) {
					dead = true;
				}
			}
			break;

		case 2: // Fantasma izquierda
			if (player->getposition_x() + 1 == ghosts[i].getposition_x() && player->getposition_y() == ghosts[i].getposition_y()) {
				if (abs(abs(player->get_movedrawxoff()) - abs(ghosts[i].get_movedrawxoff())) >= 10) {
					dead = true;
				}
			}
			break;

		case 3: // Fantasma arriba
			if (player->getposition_x() == ghosts[i].getposition_x() && player->getposition_y() + 1 == ghosts[i].getposition_y()) {
				if (abs(abs(player->get_movedrawyoff()) - abs(ghosts[i].get_movedrawyoff())) >= 10) {
					dead = true;
				}
			}
			break;
		}
	}

	if (dead) {
		pauseAll(ghosts, player);
		player->death_start();
	}
}

int main()
{
	srand(time(NULL));

    // Variables
    int ticks = 0, death_ticks = 0;
    bool began = false;

    string level_name = "nivel_1";

	// Inicializacion de ventana
    sf::RenderWindow window(sf::VideoMode(1280, 720), "PACMAN PRO");
    window.setFramerateLimit(60);

    // Mapa
    Mapa obj_mapa(&window);
    obj_mapa.load_level(level_name);

    // Textura del jugador
    sf::Texture charset_texture;
    charset_texture.loadFromFile("assets/charset.png");

    // Textura fantasmas
    sf::Texture ghosts_texture;
    ghosts_texture.loadFromFile("assets/charset_fantasmas.png");

    // Objeto pacman (jugador)
    Pacman player(&window, &charset_texture, &obj_mapa.mapa_pos);
    player.setposition(obj_mapa.pac_spawn_x, obj_mapa.pac_spawn_y);
    player.set_mapsize(obj_mapa.get_tam_x(), obj_mapa.get_tam_y());

    // Fantasmas
    vector<Fantasmita> ghosts;

    // Mapa -> Cargar fantasmas
    obj_mapa.load_ghosts(ghosts, &ghosts_texture);

    // Posicion de dibujado
    int draw_xoff = 320;
    int draw_yoff = 40;

    setDrawOffset(&player, ghosts, &obj_mapa, draw_xoff, draw_yoff);

    sf::Font font;
    font.loadFromFile("assets/fonts/emulogic.ttf");

    sf::Text text_score("", font, 16);
    text_score.setPosition(sf::Vector2f(320, 16));

    // Texto "Ready!"
    sf::Text text_ready("Ready!", font, 16);
    text_ready.setFillColor(sf::Color::Yellow);
    text_ready.setOrigin(sf::Vector2f(text_ready.getLocalBounds().width / 2, 0));
    text_ready.setPosition(sf::Vector2f(window.getSize().x / 2 + 16, 16));

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed && began)
                switch(event.key.code) {
                case sf::Keyboard::W:
                    player.rotar(3);
                    break;

                case sf::Keyboard::A:
                    player.rotar(2);
                    break;

                case sf::Keyboard::S:
                    player.rotar(1);
                    break;

                case sf::Keyboard::D:
                    player.rotar(0);
                    break;

				case sf::Keyboard::X:
					pauseAll(ghosts, &player);
					player.death_start();
					break;

				case sf::Keyboard::Escape:
					window.close();
					break;

                default: break;
                }

        }

        window.clear();

        obj_mapa.dibujar_mapa();

        // Muerte jugador
        if (player.dead) {
			death_ticks++;

			if (death_ticks >= 8) {
				death_ticks = 0;

				if (player.process_death()) {
					ghosts.clear();

					obj_mapa.load_level(level_name);
					obj_mapa.load_ghosts(ghosts, &ghosts_texture);

					player.set_mapsize(obj_mapa.get_tam_x(), obj_mapa.get_tam_y());

					player.reset();
					player.setposition(obj_mapa.pac_spawn_x, obj_mapa.pac_spawn_y);

					setDrawOffset(&player, ghosts, &obj_mapa, draw_xoff, draw_yoff);

					began = false;
					ticks = 0;
				}
			}
        } else {
			checkCollision(&player, ghosts);
        }

        player.dibujar();
        player.mover();

        // Efecto bolita especial
        if (player.ate_special) {
			player.ate_special = false;

			for (unsigned int i = 0; i < ghosts.size(); i++) {
				ghosts[i].toggle_slowmode();
			}
        }

	 	for (unsigned int i = 0; i < ghosts.size(); i++) {
			ghosts[i].dibujar();
			ghosts[i].dib_mover();
			ghosts[i].process_slowmode();
			ghosts[i].process_activation();
        }

        text_score.setString("Score: " + to_string(player.score_player));
        window.draw(text_score);

        if (ticks++ >= 240 && !began) {
            player.start();
            for (unsigned int i = 0; i < ghosts.size(); i++) {
				ghosts[i].start();
            }
            began = true;
        }

        if (!began) window.draw(text_ready);

        window.display();
    }

    return 0;
}
