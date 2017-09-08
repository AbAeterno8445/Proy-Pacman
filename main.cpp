#include <SFML/Graphics.hpp>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <string>

using namespace std;

#define random(A,B) (A + (rand() % (int)(B-A+1)))

inline bool es_pared(int tipo) {
    if (tipo == 0 || tipo == 18 || tipo == 19 || tipo == 28 || tipo == 29 || tipo == 30) {
        return false;
    }
    return true;
}

class Bolitas_Especiales {
    int bolita_esp_x;
    int bolita_esp_y;
};

class Pacman {

    //El numero del pacman es el 69

    int sprite_id;
    int sprite_animstate;

    int direction;
    float speed;

    int anim_current;
    int anim_movement[4] = { 0, 1, 2, 1 };

    bool moving;
    float move_drawoffx;
    float move_drawoffy;

    int move_queue;

	// Posicion de dibujado
    float draw_x;
    float draw_y;
    float draw_xoff;
    float draw_yoff;

    // Posicion en el mapa
    int mapa_x;
    int mapa_y;

    int** mapa_matriz;

    sf::Texture* texture_pt;
    sf::Sprite sprite;

    sf::RenderWindow* window;

    void grab_pellet() {
        score_player += 100;
    }

public:

    // Puntuaje
    int score_player;

    // Constructor
    Pacman(sf::RenderWindow* window_param, sf::Texture* texture_param, int** mapa_matriz_param) {

    	window = window_param;

    	score_player = 0;

    	mapa_matriz = mapa_matriz_param;

        texture_pt = texture_param;
        sprite.setTexture(*texture_pt);
        sprite.setScale(2, 2);
        sprite.setOrigin(sf::Vector2f(8, 8));

        sprite_id = 0;
        sprite_animstate = 0;

        anim_current = 0;

        mapa_x = 1;
        mapa_y = 1;

        direction = 0;
        speed = 1.7;

        moving = true;
        move_drawoffx = 0;
        move_drawoffy = 0;

        move_queue = 0;
    }

    void dibujar() {

        if (sprite_animstate++ == 3) {
            sprite_animstate = 0;

            if (moving) {
                sprite_id = anim_movement[anim_current];
                anim_current++;

                if (anim_current == 4) {
                    anim_current = 0;
                }
            }
        }

		sprite.setTextureRect(sf::IntRect((sprite_id % 14) * 16, floor(sprite_id / 14) * 16, 16, 16));

    	draw_x = mapa_x * 32 + 16 + move_drawoffx + draw_xoff;
    	draw_y = mapa_y * 32 + 16 + move_drawoffy + draw_yoff;

    	sprite.setPosition(sf::Vector2f(draw_x, draw_y));
		window->draw(sprite);
    }

    void set_drawoffset(int xoff_param, int yoff_param) {
        draw_xoff = xoff_param;
        draw_yoff = yoff_param;
    }

    void rotar(int dir) {
        move_queue = dir;

        if (!moving) {
            direction = dir;
            moving = true;
        }
    }

    void mover() {
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

                if (mapa_matriz[mapa_x][mapa_y] == 30) {
                    mapa_matriz[mapa_x][mapa_y] = 0;
                    grab_pellet();
                }
            }

            sprite.setRotation(direction*90);
        }
    }

    void setposition(int x, int y) {
        mapa_x = x;
        mapa_y = y;
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

	// Dibujar una sola pared
	void dibujar_pared(int x, int y, int tipo) {
	    if (tipo == 0) {
            sprite_paredes.setTextureRect(sf::IntRect(32, 32, 32, 32));
	    } else {
            tipo--;
            sprite_paredes.setTextureRect(sf::IntRect(32 * (tipo % 10), 32 * floor(tipo / 10), 32, 32));
	    }

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

		mapa_pos = new int*[tam_x];
		for (int i = 0; i < tam_x; i++) {
			mapa_pos[i] = new int[tam_y];
		}

		fstream mapfile;
		mapfile.open("mapas/mapa1.txt");
		if (mapfile.is_open()) {
            string sub_line;
            int i = 0, j = 0;
            while(getline(mapfile, sub_line, ',')) {
                int pos = atoi(sub_line.c_str());

                if (pos == 69) {
                    pac_spawn_x = i;
                    pac_spawn_y = j;
                    mapa_pos[i][j] = 0;
                } else {
                    mapa_pos[i][j] = pos;
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
	}
};

/** FUNCIONES **/

void setDrawOffset(Pacman* pacman, Mapa* mapa, int xoff, int yoff) {
    pacman->set_drawoffset(xoff, yoff);
    mapa->set_drawoffset(xoff, yoff);
};

int main()
{
	// Inicializacion de ventana

    sf::RenderWindow window(sf::VideoMode(1280, 720), "PACMAN PRO");
    window.setFramerateLimit(60);

    // Mapa

    Mapa obj_mapa(21, 20, &window);

    // Textura del jugador

    sf::Texture charset_texture;
    charset_texture.loadFromFile("assets/charset.png");

    // Objeto pacman (jugador)

    Pacman player(&window, &charset_texture, obj_mapa.mapa_pos);
    player.setposition(obj_mapa.pac_spawn_x, obj_mapa.pac_spawn_y);

    setDrawOffset(&player, &obj_mapa, 320, 40);

    sf::Font font_score;
    font_score.loadFromFile("assets/fonts/monobit.ttf");

    sf::Text text_score("", font_score, 30);
    text_score.setPosition(sf::Vector2f(320, 0));

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed)
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

        text_score.setString("Score: " + to_string(player.score_player));
        window.draw(text_score);

        window.display();
    }

    return 0;
}
