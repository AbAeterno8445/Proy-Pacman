#include <SFML/Graphics.hpp>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <iostream>

using namespace std;

#define random(A,B) (A + (rand() % (int)(B-A+1)))

/** CONSTANTES **/

// 40, 48, 88, 145

int lista_objetos[] = { 40, 48, 88, 145 };

inline bool es_pared(int tipo, bool bloque_paso) {
    if (tipo == 11 || tipo == 19 || tipo == 27 || tipo == 28 || tipo == 29 || (!bloque_paso && tipo == 18) || tipo >= 40) {
        return false;
    }
    return true;
}

/** DIBUJABLE **/

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
		if (anim_frames > 1) {
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
                    case 0:
                        if (!es_pared((*mapa_matriz)[(mapa_x + 1) + mapa_y * tam_x], move_considerpassblock)) {
                            if (mapa_x > 0 && mapa_y > 0 && mapa_x < tam_x - 1 && mapa_y < tam_y - 1) {
                                direction = move_queue;
                            }
                        }
                        break;

                    case 1:
                        if (!es_pared((*mapa_matriz)[mapa_x + (mapa_y + 1) * tam_x], move_considerpassblock)) {
                            if (mapa_x > 0 && mapa_y > 0 && mapa_x < tam_x - 1 && mapa_y < tam_y - 1) {
                                direction = move_queue;
                            }
                        }
                        break;

                    case 2:
                        if (!es_pared((*mapa_matriz)[(mapa_x - 1) + mapa_y * tam_x], move_considerpassblock)) {
                            if (mapa_x > 0 && mapa_y > 0 && mapa_x < tam_x - 1 && mapa_y < tam_y - 1) {
                                direction = move_queue;
                            }
                        }
                        break;

                    case 3:
                        if (!es_pared((*mapa_matriz)[mapa_x + (mapa_y - 1) * tam_x], move_considerpassblock)) {
                            if (mapa_x > 0 && mapa_y > 0 && mapa_x < tam_x - 1 && mapa_y < tam_y - 1) {
                                direction = move_queue;
                            }
                        }
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

    float getdrawposition_x() { return mapa_x * 32 + 16 + move_drawoffx + draw_xoff; }
    float getdrawposition_y() { return mapa_y * 32 + 16 + move_drawoffy + draw_yoff; }

    int get_movedrawxoff() { return move_drawoffx; }
    int get_movedrawyoff() { return move_drawoffy; }

    int get_direction() { return direction; }

    void start() { moving = true; reach_block(); }
    virtual void pause() { moving = false; }
};

/** OBJETOS **/

struct Objeto {
    int obj_id;

    // 0 -> obtenible / 1 -> activo / 2 -> pasivo
    int tipo_uso;

    int cantidad;

    sf::RectangleShape duration_bar;

    Objeto(int id, int duracion_p) {
        obj_id = id;

        tipo_uso = 0;
        cantidad = 1;

        duration_bar.setSize(sf::Vector2f(32, 4));
        duration_bar.setFillColor(sf::Color::Green);

        duracion = sf::milliseconds(duracion_p);
        clock.restart();
    }

    bool process() {
        float perc = clock.getElapsedTime().asSeconds() / duracion.asSeconds();
        float bar_len = 32.0f - 32.0f * perc;

        if (1 - perc <= 0.25f) {
            duration_bar.setFillColor(sf::Color::Red);
        } else if (1 - perc <= 0.5f) {
            duration_bar.setFillColor(sf::Color::Yellow);
        }

        duration_bar.setSize(sf::Vector2f(bar_len, 4));

        if (clock.getElapsedTime().asMilliseconds() >= duracion.asMilliseconds()) {
            return true;
        }

        return false;
    }

private:
    sf::Clock clock;
    sf::Time duracion;
};

/** PACMAN **/

class Pacman: public Dibujable {

	int ghosts_eaten;

	int lives;
	sf::Sprite lives_sprite;

	sf::Texture item_texture;
	sf::Sprite item_sprite;

	sf::Font* font;
	sf::Text item_amt_text;

	int bolitas;

	bool paused;

	vector<Objeto> objetos;

	// Pacman llega a un bloque
    void reach_block() {
    	switch((*mapa_matriz)[mapa_x + mapa_y * tam_x]) {
		case 29: // Bolita comun
			(*mapa_matriz)[mapa_x + mapa_y * tam_x] = 11;
			score_player += 10;
			bolitas++;
			break;

		case 27: // Bolitas especiales
		case 28:
			(*mapa_matriz)[mapa_x + mapa_y * tam_x] = 11;
			score_player += 50;
			ghosts_eaten = 0;
			ate_special = true;
			bolitas++;
			break;
    	}

    	if ((*mapa_matriz)[mapa_x + mapa_y * tam_x] >= 40) {
            grab_item((*mapa_matriz)[mapa_x + mapa_y * tam_x]);
    	}

    	// Traspasar el mapa
    	// Horizontal
    	if (mapa_x == 0) {
            if ((*mapa_matriz)[(tam_x - 1) + mapa_y * tam_x] == 11) {
                mapa_x = tam_x - 1;
            }
    	}
    	else if (mapa_x == (tam_x - 1)) {
            if ((*mapa_matriz)[mapa_y * tam_x] == 11) {
                mapa_x = 0;
            }
    	}
    	// Vertical
    	if (mapa_y == 0) {
            if ((*mapa_matriz)[mapa_x + (tam_y - 1) * tam_x] == 11) {
                mapa_y = tam_y - 1;
            }
    	}
    	else if (mapa_y == (tam_y - 1)) {
            if ((*mapa_matriz)[mapa_x] == 11) {
                mapa_y = 0;
            }
    	}
    }

    void grab_item(int item_id) {
        int dur = 0;
        int tipo = 0;

        switch(item_id) {
        case 88: // red bull
            tipo = 1;
            speed += 1.5;
            dur = 7000;
            score_player += 500;
            break;

        case 40:
            score_player += 1500;
            break;

        case 48:
            if (lives == 3) {
                score_player += 3000;
            } else {
                score_player += 1500;
                lives++;
            }
            break;

        case 145:
            tipo = 2;
            speed += 0.5;
            score_player += 500;
            break;
        }

        if (tipo != 0) {
            bool add = true;

            if (tipo == 2) {
                for(unsigned int i = 0; i < objetos.size(); i++) {
                    if (objetos[i].obj_id == item_id) {
                        objetos[i].cantidad++;
                        add = false;
                    }
                }
            }

            if (add) {
                Objeto obj_temp((*mapa_matriz)[mapa_x + mapa_y * tam_x], dur);
                obj_temp.tipo_uso = tipo;

                objetos.push_back(obj_temp);
            }
        }

        (*mapa_matriz)[mapa_x + mapa_y * tam_x] = 19;
    }

    void undo_item(int item_id) {
        switch(item_id) {
        case 88: // Red Bull
            speed -= 1.5;
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

    Pacman(sf::RenderWindow* window_param, sf::Texture* texture_param, vector<int>* mapa_matriz_param, sf::Font* font_p) : Dibujable(window_param, texture_param, mapa_matriz_param) {

    	score_player = 0;
    	ghosts_eaten = 0;

    	bolitas = 0;

    	lives = 3;
    	lives_sprite.setTexture(*texture_param);
    	lives_sprite.setScale(2, 2);
    	lives_sprite.setTextureRect(sf::IntRect(0, 0, 16, 16));

    	item_texture.loadFromFile("assets/items_t.png");
    	item_sprite.setTexture(item_texture);
    	item_sprite.setScale(0.4f, 0.4f);

    	font = font_p;
    	item_amt_text.setFont(*font);
    	item_amt_text.setCharacterSize(10);

    	dead = false;

        sprite_id = 2;

        ate_special = false;

        mapa_x = 1;
        mapa_y = 1;

        direction = 0;
        speed = 2;

        anim_frames = 4;

        anim_movement[0] = 0;
        anim_movement[1] = 1;
        anim_movement[2] = 2;
        anim_movement[3] = 1;

        moving = false;
        paused = false;
    }

    void pause() {
        moving = false;
        paused = true;
    }

    void reset(int reset_mode) {
		sprite_id = 2;
		direction = 0;
		move_queue = 0;
		ghosts_eaten = 0;

		switch(reset_mode) {
        case 1: // GAME OVER
            score_player = 0;
			lives = 3;
			bolitas = 0;
            break;

        case 2: // Luego de ganar nivel
			if (lives < 3) lives++;
			bolitas = 0;
            break;
		}

		for(int i = 0; i < (int)objetos.size(); i++) {
            if (objetos[i].tipo_uso == 2) continue;

            undo_item(objetos[i].obj_id);

            objetos.erase(objetos.begin() + i);
            i--;
		}

		moving = false;
		paused = false;
    }

    int get_lives() { return lives; }

    int get_bolitas() { return bolitas; }

    void dibujar_vidas() {
    	for (int i = 0; i < lives; i++) {
			lives_sprite.setPosition(sf::Vector2f(draw_xoff - 64, draw_yoff + 40 * (i + 1)));

			window->draw(lives_sprite);
    	}
    }

    void mover() {
        dib_mover();
        if (!dead) sprite.setRotation(direction * 90);
    }

    void rotar(int dir) {
    	if (dead || paused) return;

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
		lives--;
    }

    bool process_death() {
    	sprite_id++;
		if (sprite_id == 13) {
			dead = false;
			return true;
		}

		return false;
    }

    // Procesar objetos
    void process_items() {
        for(int i = 0; i < (int)objetos.size(); i++) {

            if (objetos[i].tipo_uso == 0) continue;

            int draw_itemid = objetos[i].obj_id - 40;

            item_sprite.setTextureRect(sf::IntRect((draw_itemid % 8) * 80, floor(draw_itemid / 8) * 80, 80, 80));

            int dx = draw_xoff - 64;
            int dy = draw_yoff + 40 * (lives + 2) + 40 * (i + 1);

            item_sprite.setPosition(dx, dy);
            objetos[i].duration_bar.setPosition(dx, dy + 30);

            window->draw(item_sprite);

            if (objetos[i].tipo_uso == 1) {
                window->draw(objetos[i].duration_bar);

                if(objetos[i].process()) {
                    undo_item(objetos[i].obj_id);

                    objetos.erase(objetos.begin() + i);
                    i--;
                }
            } else {
                if (objetos[i].cantidad > 1) {
                    item_amt_text.setPosition(dx + 28, dy + 20);
                    item_amt_text.setString("x" + to_string(objetos[i].cantidad));

                    window->draw(item_amt_text);
                }
            }
        }
    }

    int get_ghosts_eaten() { return ghosts_eaten; }

    void ghost_eaten() {
        if (ghosts_eaten < 4) ghosts_eaten++;
		score_player += 100 * pow(2, ghosts_eaten);
    }

    void ghost_eaten_reset() { ghosts_eaten = 0; }
};

/** FANTASMA **/

class Fantasmita : public Dibujable {

	int type;

	int ghost_spriteid;

    int blocks_passed;

    bool corner;

    bool slowed;
    int slowed_ticks;

    int active_ticks;
    int activation_ticks;
    bool past_firstblock;

    int start_x;
    int start_y;

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

    	// Traspasar el mapa
    	// Horizontal
    	if (mapa_x == 0) {
            if ((*mapa_matriz)[(tam_x - 1) + mapa_y * tam_x] == 11) {
                mapa_x = tam_x - 1;
            }
    	}
    	else if (mapa_x == (tam_x - 1)) {
            if ((*mapa_matriz)[mapa_y * tam_x] == 11) {
                mapa_x = 0;
            }
    	}
    	// Vertical
    	if (mapa_y == 0) {
            if ((*mapa_matriz)[mapa_x + (tam_y - 1) * tam_x] == 11) {
                mapa_y = tam_y - 1;
            }
    	}
    	else if (mapa_y == (tam_y - 1)) {
            if ((*mapa_matriz)[mapa_x] == 11) {
                mapa_y = 0;
            }
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

        type = 0;

        slowed = false;
        slowed_ticks = 0;

        activation_ticks = 60;
        active_ticks = 0;
        past_firstblock = false;

        start_x = 1;
        start_y = 1;
    }

    void set_type(int type_p) { type = type_p; }
    int get_type() { return type; }

    void reset() {
		mapa_x = start_x;
		mapa_y = start_y;

		blocks_passed = 0;

		speed = 1.5;
		direction = 0;
		moving = false;

		corner = false;

		slowed = false;
		slowed_ticks = 0;

		active_ticks = 0;
		past_firstblock = false;

		move_drawoffx = 0;
		move_drawoffy = 0;

		update_dir_anim();
		sprite_id = anim_movement[0];
    }

    void set_activation_ticks(int ticks) {
		activation_ticks = ticks;
    }

    void consider_passblock() {
        move_considerpassblock = true;
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

    bool process_slowmode() {
		if (slowed) {
			if (slowed_ticks > 0) {
				slowed_ticks--;
			} else {
				speed += 0.5;
				slowed = false;

				return true;
			}
			update_dir_anim();
		}

		return false;
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

    int get_startx() { return start_x; }
    int get_starty() { return start_y; }

    void set_startposition(int x, int y) {
		start_x = x;
		start_y = y;
    }
};

/** OJOS FANTASMA **/

class Ojos_Fantasma : public Dibujable {
	int tipo_fantasma;

	bool path_finished;

	vector<int> path;

public:
	// Constructor
	Ojos_Fantasma(sf::RenderWindow* window_param, sf::Texture* texture_param, vector<int>* mapa_matriz_param) : Dibujable(window_param, texture_param, mapa_matriz_param) {
		tipo_fantasma = 0;
		path_finished = false;

		anim_frames = 1;
		sprite_id = 21;

		direction = 0;
		speed = 4;

		mapa_x = 1;
		mapa_y = 1;

		tam_x = 0;
		tam_y = 0;

		sprite_columns = 12;

		move_considerpassblock = false;

		moving = false;

		update_dir_anim();
	}

	int get_ghosttype() { return tipo_fantasma; }
	void set_ghosttype(int type) { tipo_fantasma = type; }

	bool finished() { return path_finished; }

	void update_dir_anim() {
		switch(direction) {
		// Derecha
		case 0: sprite_id = 20; break;
		// Abajo
		case 1: sprite_id = 23; break;
		// Izquierda
		case 2: sprite_id = 21; break;
		// Arriba
		case 3: sprite_id = 22; break;
		}
	}

	void start_path(vector<int> path_param) {
		path = path_param;
		process_path();
	}

	void reach_block() {
		process_path();
	}

	void process_path() {
		if (path.size() > 0) {
			for (int i = -1; i <= 1; i++) {
				for (int j = -1; j <= 1; j++) {
					if ((i == 0 && j == 0) || (i != 0 && j != 0)) continue;

					if (path[0] == (mapa_x + i) + (mapa_y + j) * tam_x) {
						path.erase(path.begin());

						if (j == -1) direction = 3; // Arriba
						else if (j == 1) direction = 1; // Abajo
						else if (i == -1) direction = 2; // Izquierda
						else if (i == 1) direction = 0; // Derecha

						moving = true;
						update_dir_anim();
					}
				}
			}
		} else {
			path_finished = true;
		}
	}
};

/** NODO **/

class Nodo {
    int x;
    int y;

public:
    int fcost;
    int gcost;

    int parent_node;

    Nodo() {
        x = 0;
        y = 0;

        fcost = 0;
        gcost = 0;
        parent_node = 0;
    }

    int get_x() { return x; }
    int get_y() { return y; }

    void setposition(int x_p, int y_p) {
		x = x_p;
		y = y_p;
    }

    // Prueba
    bool clicked(int x_p, int y_p, int xoff, int yoff) {

    	int draw_x = x * 32 + xoff;
    	int draw_y = y * 32 + yoff;

		if (x_p > draw_x - 16 && x_p < draw_x + 16 &&
			y_p > draw_y - 16 && y_p < draw_y + 16) {
				return true;
		}

		return false;
	}
};

/** MAPA **/

class Mapa {
	int tam_x;
	int tam_y;

	int draw_xoff;
	int draw_yoff;

	sf::Texture textura_mapa;
	sf::Sprite sprite_paredes;

	sf::Texture textura_objetos;
	sf::Sprite sprite_objetos;

	sf::RenderWindow* window;

	sf::Clock item_timer;
	sf::Clock item_durationtimer;

	sf::Time item_spawntime;
	sf::Time item_duration;

	bool item_spawned;

	vector<int> bol_especiales;
	int bol_anim;

	int bolas_totales;

	vector<int> objetos;

	bool level_loaded;

	// Dibujar una sola pared
	void dibujar_pared(int x, int y, int tipo) {
		sprite_paredes.setTextureRect(sf::IntRect(32 * (tipo % 10), 32 * floor(tipo / 10), 32, 32));

		sprite_paredes.setPosition(sf::Vector2f(x * 32 + draw_xoff, y * 32 + draw_yoff));

		window->draw(sprite_paredes);
	}

public:

    vector<int> mapa_pos;
    vector<Nodo> mapa_nodo;

    int pac_spawn_x;
    int pac_spawn_y;

	// Constructor
	Mapa(sf::RenderWindow* window_param) {

		window = window_param;

		textura_mapa.loadFromFile("assets/walls.png");
		sprite_paredes.setTexture(textura_mapa);

		textura_objetos.loadFromFile("assets/items_t.png");
		sprite_objetos.setTexture(textura_objetos);
		sprite_objetos.setScale(0.4f, 0.4f);

		item_spawned = false;
		item_spawntime = sf::milliseconds(35000);
		item_duration = sf::milliseconds(18000);

		pac_spawn_x = 1;
		pac_spawn_y = 1;

		tam_x = 0;
		tam_y = 0;

		bol_anim = 0;

		bolas_totales = 0;

		level_loaded = false;
	}

	int get_tam_x() { return tam_x; }
	int get_tam_y() { return tam_y; }

	int get_drawxoff() { return draw_xoff; }
	int get_drawyoff() { return draw_yoff; }

	sf::RenderWindow* get_windowpt() { return window; }

	// Crear objeto en spawners de objetos
	void createItem(int pos = 0) {
        for(int i = 0; i < tam_x; i++) {
            for(int j = 0; j < tam_y; j++) {
                if (mapa_pos[i + j * tam_x] == 19 && find(objetos.begin(), objetos.end(), pos) == objetos.end()) {
                    pos = i + j * tam_x;
                    break;
                }
            }
        }

        if (pos == 0) return;

        objetos.push_back(pos);

        mapa_pos[pos] = lista_objetos[random(0, sizeof(lista_objetos) / sizeof(lista_objetos[0]) - 1)];
	}

	// Crear fantasma en posicion x, y
	// 0 -> Rojo / 1 -> Rosa / 2 -> Azul / 3 -> Naranja
	void createGhost(vector<Fantasmita>& ghosts, sf::Texture* ghost_texture, int tipo, int x, int y, int act_ticks, bool start) {
		Fantasmita ghost_temp(window, ghost_texture, &mapa_pos);

		ghost_temp.set_ghost_spriteid(tipo * 12);

		ghost_temp.set_activation_ticks(act_ticks);

		if (tipo == 0) ghost_temp.consider_passblock();

		ghost_temp.set_type(tipo);

		ghost_temp.setposition(x, y);
		ghost_temp.set_startposition(x, y);
		ghost_temp.set_drawoffset(draw_xoff, draw_yoff);

		ghost_temp.set_mapsize(tam_x, tam_y);

		if (start) ghost_temp.start();

		ghosts.push_back(ghost_temp);
	}

	// Cargar nivel
	void load_level(string lvlname) {
		fstream mapfile;
		string ruta = "mapas/" + lvlname + ".txt";

		mapfile.open(ruta.c_str());

		if (mapfile.is_open()) {
            string sub_line;

            if (level_loaded) {
				mapa_pos.clear();
				mapa_nodo.clear();
            }

            string tam_x_str, tam_y_str;
            getline(mapfile, tam_x_str);
            getline(mapfile, tam_y_str);

            tam_x = atoi(tam_x_str.c_str());
            tam_y = atoi(tam_y_str.c_str());

            bolas_totales = 0;

			int i = 0, j = 0;
            while(getline(mapfile, sub_line, ',')) {
                int pos = atoi(sub_line.c_str());

                Nodo bloque_temp;

                mapa_nodo.push_back(bloque_temp);
                mapa_nodo.back().setposition(i, j);

                switch(pos) {
				case 27: // Bolitas especiales
				case 28:
					mapa_pos.push_back(pos);
					bol_especiales.push_back(i + j * tam_x);

					bolas_totales++;
					break;

                case 29:
                    mapa_pos.push_back(pos);
                    bolas_totales++;
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
            cout << "Bolas tot:" << bolas_totales << endl;

            mapfile.close();
		}
	}

	// Cargar fantasmas en el nivel
	void load_ghosts(vector<Fantasmita>& ghosts, sf::Texture* ghost_texture) {
		for (int i = 0; i < tam_x; i++) {
			for (int j = 0; j < tam_y; j++) {
				switch(mapa_pos[i + j * tam_x]) {
				case 31: // Rojo
					createGhost(ghosts, ghost_texture, 0, i, j, 1, false);
					mapa_pos[i + j * tam_x] = 11;
					break;

				case 32: // Rosa
					createGhost(ghosts, ghost_texture, 1, i, j, 180, false);
					mapa_pos[i + j * tam_x] = 11;
					break;

				case 33: // Azul
					createGhost(ghosts, ghost_texture, 2, i, j, 420, false);
					mapa_pos[i + j * tam_x] = 11;
					break;

				case 34: // Naranja
					createGhost(ghosts, ghost_texture, 3, i, j, 540, false);
					mapa_pos[i + j * tam_x] = 11;
					break;
				}
			}
		}
	}

	// Cambiar posicion de dibujado del mapa
	void set_drawoffset(double xoff_param, double yoff_param) {
	    xoff_param = (1280 - (tam_x * 32)) / 2;
	    yoff_param = (720 - (tam_y * 32)) / 2;

		draw_xoff = xoff_param;
		draw_yoff = yoff_param;
	}

	// Dibujar el mapa
	void dibujar_mapa() {
		if (!level_loaded) return;

		for (int i = 0; i < tam_x; i++) {
			for (int j = 0; j < tam_y; j++) {
                if (mapa_pos[i + j * tam_x] == 19)
                    continue;

				dibujar_pared(i, j, mapa_pos[i + j * tam_x]);
			}
		}

		for (unsigned int i = 0; i < objetos.size(); i++) {

            if (mapa_pos[objetos[i]] == 19) {
                objetos.erase(objetos.begin() + i);
                i--;
                continue;
            }

            int spr_x = draw_xoff + (objetos[i] % tam_x) * 32;
            int spr_y = draw_yoff + ((objetos[i] - objetos[i] % tam_x) / tam_x) * 32;

            int spr_id = mapa_pos[objetos[i]] - 40;

            sprite_objetos.setPosition(spr_x, spr_y);
            sprite_objetos.setTextureRect(sf::IntRect((spr_id % 8) * 80, floor(spr_id / 8) * 80, 80, 80));

            window->draw(sprite_objetos);
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

	int get_bolas_totales() { return bolas_totales; }

	/** A STAR!! **/

	int astar_dist(int x1, int y1, int x2, int y2) {
        int dx = abs(x2 - x1);
        int dy = abs(y2 - y1);

        int minv = min(dx, dy);
        int maxv = max(dx, dy);

        int diagonalSteps = minv;
        int straightSteps = maxv - minv;

        return 14 * diagonalSteps + 10 * straightSteps;
	}

	vector<int> trace_path(int x1, int y1, int x2, int y2) {
        int start_node = x1 + y1 * tam_x;
        int target_node = x2 + y2 * tam_x;

        vector<int> built_path;

        vector<int> open_list;
        vector<int> closed_list;

        open_list.push_back(start_node);

        while(open_list.size() > 0) {
            int current_node = open_list[0];

            int current_x = current_node % tam_x;
            int current_y = (current_node - current_node % tam_x) / tam_x;

            for(unsigned int i = 0; i < open_list.size(); i++) {
                int tmp_node = open_list[i];

				int tmp_node_x = tmp_node % tam_x;
				int tmp_node_y = (tmp_node - tmp_node % tam_x) / tam_x;

                if (mapa_nodo[tmp_node].fcost < mapa_nodo[current_node].fcost ||
					(mapa_nodo[tmp_node].fcost == mapa_nodo[current_node].fcost && astar_dist(tmp_node_x, tmp_node_y, x2, y2) < astar_dist(current_x, current_y, x2, y2))) {
						current_node = tmp_node;

						current_x = tmp_node_x;
						current_y = tmp_node_y;
                }
            }

            open_list.erase(find(open_list.begin(), open_list.end(), current_node));
            closed_list.push_back(current_node);

            if (current_node == target_node) {

                int retrace_node = target_node;

                while(retrace_node != start_node) {
                    built_path.push_back(retrace_node);

					retrace_node = mapa_nodo[retrace_node].parent_node;
                }
                reverse(built_path.begin(), built_path.end());

                return built_path;
            }

            for(int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    if ((i == 0 && j == 0) || (i != 0 && j != 0)) continue;

                    int neighbor_node = (current_x + i) + (current_y + j) * tam_x;

                    if (es_pared(mapa_pos[neighbor_node], false) || find(closed_list.begin(), closed_list.end(), neighbor_node) != closed_list.end())
                        continue;

                    int moveToNeighborCost = mapa_nodo[current_node].gcost + astar_dist(current_x, current_y, current_x + i, current_y + j);

                    if (moveToNeighborCost < mapa_nodo[neighbor_node].gcost || find(open_list.begin(), open_list.end(), neighbor_node) == open_list.end()) {
                        mapa_nodo[neighbor_node].fcost = moveToNeighborCost + astar_dist(current_x + i, current_y + j, x2, y2);

                        mapa_nodo[neighbor_node].gcost = moveToNeighborCost;

                        mapa_nodo[neighbor_node].parent_node = current_node;

                        if (find(open_list.begin(), open_list.end(), neighbor_node) == open_list.end()) {
                            open_list.push_back(neighbor_node);
                        }
                    }
                }
            }
        }
        return built_path;
	}

	void process_items() {
	    if (!item_spawned) {
            if (item_timer.getElapsedTime().asMilliseconds() >= item_spawntime.asMilliseconds()) {
                item_durationtimer.restart();
                item_timer.restart();
                createItem();
                item_spawned = true;
            }
	    } else {
            if (item_timer.getElapsedTime().asMilliseconds() >= item_duration.asMilliseconds()) {
                objetos.clear();
                item_spawned = false;
            }
	    }
	}
};

/** EFECTOS **/

class Efecto {
	int draw_x;
	int draw_y;

	int draw_xoff;
	int draw_yoff;

	int sprite_id;
	int frames;
	int total_frames;
	int frames_ticker;
	int anim_speed;

	int anim_columns;

	bool anim_done;

	bool static_fx;

	sf::Sprite sprite;
	sf::Texture* texture;

	sf::RenderWindow* window;

public:
	// Constructor
	Efecto(sf::RenderWindow* window_param, sf::Texture* texture_param) {
		window = window_param;
		texture = texture_param;

		sprite.setTexture(*texture);
		sprite.setScale(2, 2);

		draw_x = 0;
		draw_y = 0;

		draw_xoff = 0;
		draw_yoff = 0;

		sprite_id = 0;
		frames = 0;
		total_frames = 0;
		frames_ticker = 0;
		anim_speed = 0;

		anim_columns = 0;

		anim_done = false;

		static_fx = false;
	}

	void set_anim(int anim_cols, int start_frame, int tot_frames, int speed, bool stat) {
		anim_columns = anim_cols;
		sprite_id = start_frame;
		total_frames = tot_frames;
		anim_speed = speed;
		static_fx = stat;
	}

	void set_drawposition(int x, int y) {
		draw_x = x;
		draw_y = y;
	}

	void set_drawoffset(int xoff, int yoff) {
		draw_xoff = xoff;
		draw_yoff = yoff;
	}

	void dibujar() {
		sprite.setPosition(sf::Vector2f(draw_x + draw_xoff, draw_y + draw_yoff));

		sprite.setTextureRect(sf::IntRect((sprite_id % anim_columns) * 16, floor(sprite_id / anim_columns) * 16, 16, 16));

		frames_ticker++;
		if (frames_ticker >= anim_speed) {
			frames_ticker = 0;

			if (!static_fx) sprite_id++;

			frames++;
			if (frames >= total_frames) {
				anim_done = true;
			}
		}

		window->draw(sprite);
	}

	bool done() { return anim_done; }
};

/** FUNCIONES **/

void setDrawOffset(Pacman* pacman, vector<Fantasmita>& ghosts, Mapa* mapa, int xoff, int yoff) {
    pacman->set_drawoffset(xoff, yoff);
    mapa->set_drawoffset(xoff, yoff);

    for (unsigned int i = 0; i < ghosts.size(); i++) {
		ghosts[i].set_drawoffset(xoff, yoff);
    }
};

void pauseAll(vector<Fantasmita>& ghosts, Pacman* player, vector<Ojos_Fantasma>& ghost_eyes) {
	for (unsigned int i = 0; i < ghosts.size(); i++) {
		ghosts[i].pause();
	}
	for (unsigned int i = 0; i < ghost_eyes.size(); i++) {
		ghost_eyes[i].pause();
	}
	player->pause();
}

void checkCollision(Pacman* player, vector<Fantasmita>& ghosts, vector<Ojos_Fantasma>& ghost_eyes, sf::Texture* ghost_texture, Mapa* mapa, vector<Efecto>& effects) {
	bool dead = false;
	bool eaten = false;

	int eaten_id = 0;

	for (unsigned int i = 0; i < ghosts.size(); i++) {
        int dist = pow( pow(ghosts[i].getdrawposition_x() - player->getdrawposition_x(), 2) + pow(ghosts[i].getdrawposition_y() - player->getdrawposition_y(), 2) , 0.5);

        if (dist <= 16) {
            if (!ghosts[i].is_slowed()) dead = true;
            else {
                eaten = true;
                eaten_id = i;
            }
        }
	}

	if (dead) {
		pauseAll(ghosts, player, ghost_eyes);
		player->death_start();
	}
	else if (eaten) {
		Ojos_Fantasma ojo_temp(mapa->get_windowpt(), ghost_texture, &mapa->mapa_pos);

		int ojo_x = ghosts[eaten_id].getposition_x();
		int ojo_y = ghosts[eaten_id].getposition_y();

		ojo_temp.setposition(ojo_x, ojo_y);
		ojo_temp.set_mapsize(mapa->get_tam_x(), mapa->get_tam_y());

		ojo_temp.set_ghosttype(ghosts[eaten_id].get_type());

		ojo_temp.set_drawoffset(mapa->get_drawxoff(), mapa->get_drawyoff());

		ojo_temp.start_path(mapa->trace_path(ojo_x, ojo_y, ghosts[eaten_id].get_startx(), ghosts[eaten_id].get_starty()));

		ghost_eyes.push_back(ojo_temp);
		ghosts.erase(ghosts.begin() + eaten_id);

		player->ghost_eaten();

		// Efecto al comer fantasma
		Efecto eaten_fx(mapa->get_windowpt(), ghost_texture);
		eaten_fx.set_anim(12, 47 + player->get_ghosts_eaten(), 1, 120, true);

		eaten_fx.set_drawoffset(mapa->get_drawxoff(), mapa->get_drawyoff());
		eaten_fx.set_drawposition(ojo_x * 32, ojo_y * 32);

		effects.push_back(eaten_fx);
	}
}

int main()
{
	srand(time(NULL));

    // Variables
    int ticks = 0, death_ticks = 0;
    bool began = false;

    string level_name = "nivel_1";

    sf::Font font;
    font.loadFromFile("assets/fonts/emulogic.ttf");

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
    Pacman player(&window, &charset_texture, &obj_mapa.mapa_pos, &font);
    player.setposition(obj_mapa.pac_spawn_x, obj_mapa.pac_spawn_y);
    player.set_mapsize(obj_mapa.get_tam_x(), obj_mapa.get_tam_y());

    // Efectos
    vector<Efecto> effects;

    // Fantasmas
    vector<Fantasmita> ghosts;

    // Ojos de fantasmas
    vector<Ojos_Fantasma> ghost_eyes;

    // Mapa -> Cargar fantasmas
    obj_mapa.load_ghosts(ghosts, &ghosts_texture);

    // Posicion de dibujado
    int draw_xoff;
    int draw_yoff;

    draw_xoff = (window.getSize().x - (obj_mapa.get_tam_x()*32)) / 2;
    draw_yoff = (window.getSize().y - (obj_mapa.get_tam_y()*32)) / 2;

    setDrawOffset(&player, ghosts, &obj_mapa, draw_xoff, draw_yoff);

    // Texto SCORE
    sf::Text text_score("", font, 16);
    text_score.setPosition(sf::Vector2f(980 , 27));

    // Texto "Ready!"
    sf::Text text_ready("Ready!", font, 16);
    text_ready.setFillColor(sf::Color::Yellow);
    text_ready.setOrigin(sf::Vector2f(text_ready.getLocalBounds().width / 2, 0));
    text_ready.setPosition(sf::Vector2f(1050 , 50));

    int gameover_ticks = 0;
    bool gameover = false;

    int won_ticks = 0;
    bool won = false;

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
                    obj_mapa.createItem();
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
        if (!gameover) {
			if (player.dead) {
				death_ticks++;

				if (death_ticks >= 8) {
					death_ticks = 0;

					if (player.process_death()) {
						if (player.get_lives() <= 0) {
							gameover = true;
						}
						else {
							for (unsigned int i = 0; i < ghosts.size(); i++) {
								ghosts[i].reset();
							}
							player.reset(0);
							player.setposition(obj_mapa.pac_spawn_x, obj_mapa.pac_spawn_y);
						}

						began = false;
						ticks = 0;
					}
				}
			} else {
				checkCollision(&player, ghosts, ghost_eyes, &ghosts_texture, &obj_mapa, effects);
			}
        } else {
        	gameover_ticks++;

        	if (gameover_ticks >= 90) {
        		text_ready.setString("Game Over!");
				window.draw(text_ready);
        	}

        	if (gameover_ticks >= 270) {
				ghosts.clear();
				ghost_eyes.clear();

				obj_mapa.load_level(level_name);
				obj_mapa.load_ghosts(ghosts, &ghosts_texture);

				player.set_mapsize(obj_mapa.get_tam_x(), obj_mapa.get_tam_y());

				player.reset(1);
				player.setposition(obj_mapa.pac_spawn_x, obj_mapa.pac_spawn_y);

				setDrawOffset(&player, ghosts, &obj_mapa, draw_xoff, draw_yoff);

				gameover = false;
				gameover_ticks = 0;

				text_ready.setString("Ready!");
        	}

        	window.display();

        	continue;
        }

        if (gameover) continue;

        obj_mapa.process_items();

        player.dibujar();
        player.dibujar_vidas();
        player.mover();
        player.process_items();

        // Ganar nivel
        if (player.get_bolitas() >= obj_mapa.get_bolas_totales() && !won) {
            pauseAll(ghosts, &player, ghost_eyes);
            won = true;
        }

        if (won) {
            won_ticks++;
            if (won_ticks >= 240) {
                won = false;
                won_ticks = 0;
                ghosts.clear();
				ghost_eyes.clear();

				obj_mapa.load_level(level_name);
				obj_mapa.load_ghosts(ghosts, &ghosts_texture);

				player.set_mapsize(obj_mapa.get_tam_x(), obj_mapa.get_tam_y());

				player.reset(2);
				player.setposition(obj_mapa.pac_spawn_x, obj_mapa.pac_spawn_y);

				setDrawOffset(&player, ghosts, &obj_mapa, draw_xoff, draw_yoff);

				began = false;
                ticks = 0;
            }
        }

        // Efecto bolita especial
        if (player.ate_special) {
			player.ate_special = false;

			for (unsigned int i = 0; i < ghosts.size(); i++) {
				ghosts[i].toggle_slowmode();
			}
        }

        // Procesar fantasmas
	 	for (unsigned int i = 0; i < ghosts.size(); i++) {
			ghosts[i].dibujar();
			ghosts[i].dib_mover();
			if (ghosts[i].process_slowmode()) {
				player.ghost_eaten_reset();
			}
			ghosts[i].process_activation();
        }

        // Procesar ojos de fantasmas
        for (unsigned int i = 0; i < ghost_eyes.size(); i++) {
			ghost_eyes[i].dibujar();
			ghost_eyes[i].dib_mover();

			if (ghost_eyes[i].finished()) {
				obj_mapa.createGhost(ghosts, &ghosts_texture, ghost_eyes[i].get_ghosttype(), ghost_eyes[i].getposition_x(), ghost_eyes[i].getposition_y(), 1, true);

				ghost_eyes.erase(ghost_eyes.begin() + i);
			}
        }

        // Procesar efectos
        for (unsigned int i = 0; i < effects.size(); i++) {
			effects[i].dibujar();

			if (effects[i].done()) effects.erase(effects.begin() + i);
        }

        text_score.setString("Score: " + to_string(player.score_player));
        window.draw(text_score);

        if (ticks++ >= 240 && !began) {
            player.start();
            for (unsigned int i = 0; i < ghosts.size(); i++) {
				ghosts[i].start();
            }
            for (unsigned int i = 0; i < ghost_eyes.size(); i++) {
				ghost_eyes[i].start();
            }
            began = true;
        }

        if (!began) window.draw(text_ready);

        window.display();
    }

    return 0;
}
