#include <SFML/Graphics.hpp>
#include <time.h>
#include <list>
#include <SFML/Audio.hpp>
#include <iostream>


using namespace sf;
using namespace std;


// Setting Window Size //
const int W = 1200;
const int H = 800;




// Degrees to Radians //
float DEGTORAD = 0.017453f;

class Animation
{
public:
	float Frame, speed;
	Sprite sprite;
	vector<IntRect> frames;

	Animation() {}

	Animation(Texture &t, int x, int y, int w, int h, int count, float Speed)
	{
		Frame = 0;
		speed = Speed;

		for (int i = 0; i<count; i++)
			frames.push_back(IntRect(x + i * w, y, w, h));

		sprite.setTexture(t);
		sprite.setOrigin(w / 2, h / 2);
		sprite.setTextureRect(frames[0]);
	}


	void update()
	{
			Frame += speed;
			int n = frames.size();
			if (Frame >= n) Frame -= n;
			if (n>0) sprite.setTextureRect(frames[int(Frame)]);
		
	}

	bool isEnd()
	{
		return Frame + speed >= frames.size();
	}

};

		// ENTITY CLASS //

class Entity
{
public:
	float x, y, dx, dy, R, angle;
	bool life;
	std::string name;
	Animation anim;

	Entity()
	{
		life = 1;
	}

	void settings(Animation &a, int X, int Y, float Angle = 0, int radius = 1)
	{
		anim = a;
		x = X; y = Y;
		angle = Angle;
		R = radius;
	}

	virtual void update() {};

	void draw(RenderWindow &app)
	{
		anim.sprite.setPosition(x, y);
		anim.sprite.setRotation(angle + 90);
		app.draw(anim.sprite);

		CircleShape circle(R);
		circle.setFillColor(Color(255, 0, 0, 170));
		circle.setPosition(x, y);
		circle.setOrigin(R, R);
		//app.draw(circle);
	}

	virtual ~Entity() {};
};

		// ASTEROID CLASS //

class asteroid : public Entity
{
public:
	asteroid()
	{
		dx = rand() % 8 - 4;
		dy = rand() % 8 - 4;
		name = "asteroid";
	}

	void  update()
	{
		x += dx;
		y += dy;

		if (x>W) x = 0;  if (x<0) x = W;
		if (y>H) y = 0;  if (y<0) y = H;
	}

};


		// BULLET CLASS //

class bullet : public Entity
{
public:
	bullet()
	{
		name = "bullet";
	}

	void  update()
	{
		dx = cos(angle*DEGTORAD) * 6;
		dy = sin(angle*DEGTORAD) * 6;
		// angle+=rand()%6-3;
		x += dx;
		y += dy;

		if (x>W || x<0 || y>H || y<0) life = 0;
	}

};


		// PLAYER CLASS //

class player : public Entity
{
public:
	bool thrust;

	player()
	{
		name = "player";
	}

	void update()
	{
		if (thrust)
		{
			dx += cos(angle*DEGTORAD)*0.2;
			dy += sin(angle*DEGTORAD)*0.2;
		}
		else
		{
			dx *= 0.99;
			dy *= 0.99;
		}

		int maxSpeed = 15;
		float speed = sqrt(dx*dx + dy * dy);
		if (speed>maxSpeed)
		{
			dx *= maxSpeed / speed;
			dy *= maxSpeed / speed;
		}

		x += dx;
		y += dy;

		if (x>W) x = 0; if (x<0) x = W;
		if (y>H) y = 0; if (y<0) y = H;
	}

};

// Collision detection //
bool isCollide(Entity *a, Entity *b)
{
	return (b->x - a->x)*(b->x - a->x) +
		(b->y - a->y)*(b->y - a->y)<
		(a->R + b->R)*(a->R + b->R);
}


int main()
{
	// Attempt at a Game Over screen //
/*	RectangleShape endScreen;
	Texture endTexture;

	endScreen.setSize(sf::Vector2f(1200, 800));
	endTexture.loadFromFile("images/maxresdefault.jpg");
	endScreen.setPosition(2000, 2000);
	endScreen.setTexture(&endTexture);
*/

	// Music //

	Music music;
	music.openFromFile("images/Star Wars- The Imperial March (Darth Vader's Theme).ogg");

	music.play();
	music.setLoop(true);

	// Sound Effect //

	SoundBuffer buffer;
	buffer.loadFromFile("images/pew.wav");

	Sound sound;
	sound.setBuffer(buffer);
	sound.setVolume(25);



	// Lifes and Score //
	int lives = 3;
	int score = 0;

	// Pause //
	bool isPaused = false;

	Font hudFont;
	if (!hudFont.loadFromFile("images/Fonts/Magettas Regular DEMO.otf"))
	return 0;

	// setting the text HUD //
	Text lifeLabel;
	lifeLabel.setFont(hudFont);
	lifeLabel.setString("Lives Remaining: ");
	lifeLabel.setCharacterSize(16);
	lifeLabel.setPosition(10, 10);
	lifeLabel.setFillColor(sf::Color::White);
	
	// setting the score HUD //
	Text scoreLabel;
	scoreLabel.setFont(hudFont);
	scoreLabel.setString("Score: ");
	scoreLabel.setCharacterSize(16);
	scoreLabel.setPosition(10, 40);
	scoreLabel.setFillColor(sf::Color::White);


	// Setting the text for the lives //
	Text livesLeft;
	livesLeft.setFont(hudFont);
	livesLeft.setString(to_string(lives));
	livesLeft.setCharacterSize(16);
	livesLeft.setPosition(145, 10);
	livesLeft.setFillColor(sf::Color::White);


	// Setting the text for the Score //
	Text totalScore;
	totalScore.setFont(hudFont);
	totalScore.setString(to_string(score));
	totalScore.setCharacterSize(16);
	totalScore.setPosition(65, 40);
	totalScore.setFillColor(sf::Color::White);


	

srand(time(0));


		// Window rendering and naming //

	RenderWindow app(VideoMode(W, H), "Asteroids!"); 
	app.setFramerateLimit(60);


		// Textures
	

	Texture  t1, t2, t3, t4, t5, t6, t7, t8;

	t1.loadFromFile("images/spaceship.jpeg");
	t2.loadFromFile("images/background.jpeg");
	t3.loadFromFile("images/explosions/type_C.png");
	t4.loadFromFile("images/rock.png");
	t5.loadFromFile("images/fire_blue.png");
	t6.loadFromFile("images/rock_small.png");
	t7.loadFromFile("images/explosions/type_B.png");
	t8.loadFromFile("images/maxresdefault.jpg");
	


		// makes textures less pixelated //
	t1.setSmooth(true);
	t2.setSmooth(true);

	Sprite background(t2);


		// ANIMATIONS //

	Animation sExplosion(t3, 0, 0, 256, 256, 48, 0.5);
	Animation sRock(t4, 0, 0, 64, 64, 16, 0.2);
	Animation sRock_small(t6, 0, 0, 64, 64, 16, 0.2);
	Animation sBullet(t5, 0, 0, 32, 64, 16, 0.8);
	Animation sPlayer(t1, 40, 0, 40, 40, 1, 0);
	Animation sPlayer_go(t1, 40, 40, 40, 40, 1, 0);
	Animation sExplosion_ship(t7, 0, 0, 192, 192, 64, 0.5);


	std::list<Entity*> entities;

	for (int i = 0; i<15; i++)
	{
		asteroid *a = new asteroid();
		a->settings(sRock, rand() % W, rand() % H, rand() % 360, 25);
		entities.push_back(a);
	}

	player *p = new player();
	p->settings(sPlayer, 200, 200, 0, 20);
	entities.push_back(p);

		// Main loop //

	while (app.isOpen())
	{
		Event event;
		while (app.pollEvent(event))
		{
			if (event.type == Event::Closed)
				app.close();


			// Fire control and logic //
			if (event.type == Event::KeyPressed)
				if (event.key.code == Keyboard::Space)
				{	
					bullet *b = new bullet();
					b->settings(sBullet, p->x, p->y, p->angle, 10);
					entities.push_back(b);
				}

		}


		// Movement controls //
		if (Keyboard::isKeyPressed(Keyboard::Space)) sound.play();
		if (Keyboard::isKeyPressed(Keyboard::Right)) p->angle += 3;
		if (Keyboard::isKeyPressed(Keyboard::Left))  p->angle -= 3;
		if (Keyboard::isKeyPressed(Keyboard::Up)) p->thrust = true;
		else p->thrust = false;

		// Pause (work in progress) //
	
		bool pause = false;

		if (Keyboard::isKeyPressed(Keyboard::P))
		{
			if (pause == false)
			{
				pause = true;
				system("pause");
			}										// WORKS (KINDA), BUT AFTER YOU PAUSE YOU NEED TO CLICK ON THE CONSOL AND PRESS ANY KEY TO UNPAUSE //
		}
		if (Keyboard::isKeyPressed(Keyboard::O))
		{
			if (pause == true)								
			{
				pause == false;
			}
		}

		// Quit Game //

		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			app.close();
		}



		// Collision Detection //

		for (auto a : entities)
			for (auto b : entities)
			{
				if (a->name == "asteroid" && b->name == "bullet")
					if (isCollide(a, b))
					{
						a->life = false;
						b->life = false;

						score++;
						totalScore.setString(to_string(score));

						// Adding the explosion animation //
						Entity *e = new Entity();
						e->settings(sExplosion, a->x, a->y);
						e->name = "explosion";
						entities.push_back(e);


						// Creating smaller rocks from destroyed rocks //
						for (int i = 0; i<2; i++)
						{
							if (a->R == 15) continue;
							Entity *e = new asteroid();
							e->settings(sRock_small, a->x, a->y, rand() % 360, 15);
							entities.push_back(e);
						}

					}


					// Death when colliding with an Asteroid //

				if (a->name == "player" && b->name == "asteroid")
					if (isCollide(a, b))
					{
						b->life = false;

						Entity *e = new Entity();
						e->settings(sExplosion_ship, a->x, a->y);
						e->name = "explosion";
						entities.push_back(e);

						// Player respawn point //
						p->settings(sPlayer, W / 2, H / 2, 0, 20);
						p->dx = 0; p->dy = 0;

						// Lives are reduced on death, if lives reach 0, pause screen //
						lives--;
						livesLeft.setString(to_string(lives));
						if (lives < 0)
						{
							cout << "      GAME OVER" << endl << endl;
							system("pause");
							app.close();
							
						}

					}
			}


		 // Player //
		if (p->thrust)  p->anim = sPlayer_go;
		else   p->anim = sPlayer;

		// Animation for the asteroid explosion //
		for (auto e : entities)
			if (e->name == "explosion")
				if (e->anim.isEnd()) e->life = 0;


		// Creating smaller asteroids from destroyed ones //
		if (rand() % 150 == 0)
		{
			asteroid *a = new asteroid();
			a->settings(sRock, 0, rand() % H, rand() % 360, 25);
			entities.push_back(a);
		}

		for (auto i = entities.begin(); i != entities.end();)
		{
			Entity *e = *i;

			e->update();
			e->anim.update();

			if (e->life == false) { i = entities.erase(i); delete e; }
			else i++;
		}



		// Draw //
		//app.draw(endScreen);
		app.draw(background);
		app.draw(lifeLabel);
		app.draw(livesLeft);
		app.draw(scoreLabel);
		app.draw(totalScore);
		

		for (auto i : entities)
			i->draw(app);

		app.display();
	}

	return 0;
}
