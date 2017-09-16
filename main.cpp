#include <SFML/Graphics.hpp>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <string>

using namespace std;

#define random(A,B) (A + (rand() % (int)(B-A+1)))

inline bool es_pared(int tipo) {
    if (tipo == 11 || tipo == 18 || tipo == 19 || tipo == 27 || tipo == 28 || tipo == 29) {
        return false;
    }
    return true;
}

class Dibujable {
protected:
    sf::Texture* texture_pt;
    sf::Sprite sprite;

    sf::RenderWindow* window;

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

    int anim_speed;

    int** mapa_matriz;

    virtual void reach_block() {};
    virtual void ghost_corner() {};

public:
    //Constructor

    Dibujable(sf::RenderWindow* window_param, sf::Texture* texture_param, int** mapa_matriz_param) {
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
    }

    virtual ~Dibujable() {}

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
                if (!es_pared(mapa_matriz[mapa_x + 1][mapa_y])) {
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
                if (!es_pared(mapa_matriz[mapa_x][mapa_y + 1])) {
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
                if (!es_pared(mapa_matriz[mapa_x - 1][mapa_y])) {
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
                if (!es_pared(mapa_matriz[mapa_x][mapa_y - 1])) {
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
                    case 0: if (!es_pared(mapa_matriz[mapa_x + 1][mapa_y])) direction = move_queue;
                        break;

                    case 1: if (!es_pared(mapa_matriz[mapa_x][mapa_y + 1])) direction = move_queue;
                        break;

                    case 2: if (!es_pared(mapa_matriz[mapa_x - 1][mapa_y])) direction = move_queue;
                        break;

                    case 3: if (!es_pared(mapa_matriz[mapa_x][mapa_y - 1])) direction = move_queue;
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
        mapa_x = x;
        mapa_y = y;
    }
};

class Pacman: public Dibujable {

    //El numero del pacman es el 69

    void reach_block() {
    	switch(mapa_matriz[mapa_x][mapa_y]) {
		case 29: // Bolita comun
			mapa_matriz[mapa_x][mapa_y] = 11;
			score_player += 100;
			break;

		case 27: // Bolitas especiales
		case 28:
			mapa_matriz[mapa_x][mapa_y] = 11;
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

    Pacman(sf::RenderWindow* window_param, sf::Texture* texture_param, int** mapa_matriz_param) : Dibujable(window_param, texture_param, mapa_matriz_param) {

    	score_player = 0;

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

    void mover() {
        dib_mover();
        sprite.setRotation(direction * 90);
    }

    void rotar(int dir) {
        move_queue = dir;

        if (!moving) {
            direction = dir;
            moving = true;
        }
    }

    void start() {
        moving = true;
    }
};

class Fantasmita : public Dibujable {

	int ghost_spriteid;

    int blocks_passed;

    bool corner;

    bool slowed;
    int slowed_ticks;

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
				if (!es_pared(mapa_matriz[mapa_x + xx][mapa_y + yy])) {
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
    	blocks_passed++;

    	// Si existe la posibilidad de cambiar de eje de movimiento, aumentar la chance
		switch(direction) {
		case 0: // Horizontal
		case 2:
			if (!es_pared(mapa_matriz[mapa_x][mapa_y + 1]) || !es_pared(mapa_matriz[mapa_x][mapa_y - 1])) {
				blocks_passed += 6;
			}
			break;

		case 1: // Vertical
		case 3:
			if (!es_pared(mapa_matriz[mapa_x + 1][mapa_y]) || !es_pared(mapa_matriz[mapa_x - 1][mapa_y])) {
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
    Fantasmita(sf::RenderWindow* window_param, sf::Texture* texture_param, int** mapa_matriz_param) : Dibujable(window_param, texture_param, mapa_matriz_param) {

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
    }

    void set_ghost_spriteid(int id) {
		ghost_spriteid = id;
		sprite_id = id;
		update_dir_anim();
    }

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
				update_dir_anim();
			}
		}
    }

    void start() {
		moving = true;
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

	// Dibujar una sola pared
	void dibujar_pared(int x, int y, int tipo) {
		sprite_paredes.setTextureRect(sf::IntRect(32 * (tipo % 10), 32 * floor(tipo / 10), 32, 32));

		sprite_paredes.setPosition(sf::Vector2f(x * 32 + draw_xoff, y * 32 + draw_yoff));

		window->draw(sprite_paredes);
	}

	public:

    int** mapa_pos;

    int pac_spawn_x;
    int pac_spawn_y;

	// Constructor
	Mapa(int tam_x_param, int tam_y_param, sf::RenderWindow* window_param) {

		window = window_param;

		textura_mapa.loadFromFile("assets/walls.png");
		sprite_paredes.setTexture(textura_mapa);

		tam_x = tam_x_param;
		tam_y = tam_y_param;

		pac_spawn_x = 1;
		pac_spawn_y = 1;

		bol_anim = 0;

		mapa_pos = new int*[tam_x];
		for (int i = 0; i < tam_x; i++) {
			mapa_pos[i] = new int[tam_y];
		}

		fstream mapfile;
		mapfile.open("mapas/nivel_1.txt");

		if (mapfile.is_open()) {
            string sub_line;
            int i = 0, j = 0;

            string tam_x_str, tam_y_str;
            getline(mapfile, tam_x_str);
            getline(mapfile, tam_y_str);

            tam_x = atoi(tam_x_str.c_str());
            tam_y = atoi(tam_y_str.c_str());

            while(getline(mapfile, sub_line, ',')) {
                int pos = atoi(sub_line.c_str());

                switch(pos) {
				case 27: // Bolitas especiales
				case 28:
					mapa_pos[i][j] = pos;
					bol_especiales.push_back(i + j * tam_x);
					break;

				case 30: // Posicion inicial pacman
					pac_spawn_x = i;
                    pac_spawn_y = j;
                    mapa_pos[i][j] = 11;
					break;

				default:
					mapa_pos[i][j] = pos;
					break;
                }

                i++;
                if(i == tam_x) {
                    i = 0;
                    j++;
                }
            }

            mapfile.close();
		}
	}

	// Cambiar posicion de dibujado del mapa
	void set_drawoffset(int xoff_param, int yoff_param) {
		draw_xoff = xoff_param;
		draw_yoff = yoff_param;
	}

	// Dibujar el mapa
	void dibujar_mapa() {
		for (int i = 0; i < tam_x; i++) {
			for (int j = 0; j < tam_y; j++) {
				dibujar_pared(i, j, mapa_pos[i][j]);
			}
		}

		if (bol_anim++ == 3) {
			bol_anim = 0;

			for(unsigned int i = 0; i < bol_especiales.size(); i++) {
				int pos = mapa_pos[bol_especiales[i] % tam_x][(bol_especiales[i] - bol_especiales[i] % tam_x) / tam_x];

				if (pos == 27) {
					mapa_pos[bol_especiales[i] % tam_x][(bol_especiales[i] - bol_especiales[i] % tam_x) / tam_x] = 28;
				} else if (pos == 28) {
					mapa_pos[bol_especiales[i] % tam_x][(bol_especiales[i] - bol_especiales[i] % tam_x) / tam_x] = 27;
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

int main()
{
    // Variables

    int ticks=0;
    bool began = false;

	// Inicializacion de ventana

    sf::RenderWindow window(sf::VideoMode(1280, 720), "PACMAN PRO");
    window.setFramerateLimit(60);

    // Mapa

    Mapa obj_mapa(21, 20, &window);

    // Textura del jugador

    sf::Texture charset_texture;
    charset_texture.loadFromFile("assets/charset.png");

    // Textura fantasmas

    sf::Texture ghosts_texture;
    ghosts_texture.loadFromFile("assets/charset_fantasmas.png");

    // Objeto pacman (jugador)

    Pacman player(&window, &charset_texture, obj_mapa.mapa_pos);
    player.setposition(obj_mapa.pac_spawn_x, obj_mapa.pac_spawn_y);

    // Fantasmas

    vector<Fantasmita> ghosts;

    // Rojo
    Fantasmita* ghost_temp = new Fantasmita(&window, &ghosts_texture, obj_mapa.mapa_pos);
    ghost_temp->setposition(10, 6);
    ghosts.push_back(*ghost_temp);

    // Rosa
    ghost_temp->setposition(11, 8);
    ghost_temp->set_ghost_spriteid(12);
    ghosts.push_back(*ghost_temp);

    // Azul
    ghost_temp->setposition(9, 8);
    ghost_temp->set_ghost_spriteid(24);
    ghosts.push_back(*ghost_temp);

    // Naranja
    ghost_temp->setposition(10, 8);
    ghost_temp->set_ghost_spriteid(36);
    ghosts.push_back(*ghost_temp);

    delete ghost_temp;

    // Posicion de dibujado
    setDrawOffset(&player, ghosts, &obj_mapa, 320, 40);

    sf::Font font;
    font.loadFromFile("assets/fonts/emulogic.ttf");

    sf::Text text_score("", font, 16);
    text_score.setPosition(sf::Vector2f(320, 16));

    // Texto "Ready!"
    sf::Text text_ready("Ready!", font, 16);
    text_ready.setFillColor(sf::Color::Yellow);
    text_ready.setOrigin(sf::Vector2f(text_ready.getLocalBounds().width / 2, 0));
    text_ready.setPosition(sf::Vector2f(window.getSize().x / 2 + 16, window.getSize().y / 2 + 8));

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

                default: break;
                }

        }

        window.clear();

        obj_mapa.dibujar_mapa();
        player.dibujar();
        player.mover();

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
