#ifndef BUNNY_H
#define BUNNY_H

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f

class Bunny : public Test
{
public:
	b2Vec2 beforePosition;

	Bunny()
	{
		forward = 0.0;
		backward = 0.0;
		//Åä³¢
		{
			b2PolygonShape rbody;
			b2Vec2 vertices[8];
			vertices[0].Set(-2.0f, 1.0f);
			vertices[1].Set(-1.5f, 0.3f);
			vertices[2].Set(-1.0f, 0.0f); //back
			vertices[3].Set(1.0f, 0.0f); //front
			vertices[4].Set(1.5f, 0.3f);
			vertices[5].Set(2.0f, 1.0f);
			vertices[6].Set(1.0f, 2.0f);
			vertices[7].Set(-1.0f, 2.0f);
			rbody.Set(vertices, 8);

			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			bd.position.Set(0.0f, 1.0f);
			beforePosition = { 0.0f, 1.0f };

			b2FixtureDef fd;
			fd.shape = &rbody;
			fd.density = 2.0f;
			fd.friction = 7.0f;

			rabbit = m_world->CreateBody(&bd);
			rabbit->CreateFixture(&rbody, 1.0f);

		}
		//¹ÙÀ§ ^^
		{
			b2PolygonShape rockshape;
			rockshape.SetAsBox(1.0f, 1.0f);

			b2Body* rock = NULL;
			b2BodyDef bd;
			bd.type = b2_dynamicBody;

			b2FixtureDef fd;
			fd.shape = &rockshape;
			fd.density = 0.7f;
			fd.friction = 0.5f;
			fd.restitution = 0.4f;

			bd.position.Set(10.0f, 0.5f);
			rock = m_world->CreateBody(&bd);
			rock->CreateFixture(&fd);

			bd.position.Set(13.0f, 0.5f);
			rock = m_world->CreateBody(&bd);
			rock->CreateFixture(&fd);
		}

		//¶¥
		b2Body* ground = NULL;
		{
			b2BodyDef bd;
			ground = m_world->CreateBody(&bd);


			b2EdgeShape shape;

			b2FixtureDef fd;
			fd.shape = &shape;
			fd.friction = 0.9f;
			fd.restitution = 0.1f;

			shape.Set(b2Vec2(-20.0f, 0.0f), b2Vec2(20.0f, 0.0f));
			ground->CreateFixture(&fd);

			float32 hills[10] = { 0.25f, 10.0f, 4.0f, 0.0f, -5.0f, -1.0f, -2.0f, -2.0f, -1.25f, 8.0f };
			float32 x = 20.0f, y1 = 0.0f, dx = 5.0f;

			for (int32 i = 0; i < 10; ++i)
			{
				float32 y2 = hills[i];
				shape.Set(b2Vec2(x, y1), b2Vec2(x + dx, y2));
				ground->CreateFixture(&fd);
				y1 = y2;
				x += dx;
			}

			for (int32 i = 0; i < 10; ++i)
			{
				float32 y2 = hills[i];
				shape.Set(b2Vec2(x, y1), b2Vec2(x + dx, y2));
				ground->CreateFixture(&fd);
				y1 = y2;
				x += dx;
			}

			shape.Set(b2Vec2(x, 0.0f), b2Vec2(x + 40.0f, 0.0f));
			ground->CreateFixture(&fd);

			x += 80.0f;
			shape.Set(b2Vec2(x, 0.0f), b2Vec2(x + 40.0f, 0.0f));
			ground->CreateFixture(&fd);

			x += 40.0f;
			shape.Set(b2Vec2(x, 0.0f), b2Vec2(x + 10.0f, 5.0f));
			ground->CreateFixture(&fd);

			x += 20.0f;
			shape.Set(b2Vec2(x, 0.0f), b2Vec2(x + 40.0f, 0.0f));
			ground->CreateFixture(&fd);

			x += 40.0f;
			shape.Set(b2Vec2(x, 0.0f), b2Vec2(x, 20.0f));
			ground->CreateFixture(&fd);
		}
	} //draw something
	void Keyboard(int key)
	{
		switch (key)
		{
		case GLFW_KEY_A:
			backward += 1.0f;
			break;
		case GLFW_KEY_D:
			forward += 1.0f;
			break;
		case GLFW_KEY_W:
			if (!jumpAvail)
			{
				beforePosition = rabbit->GetPosition();
				jumpstep = 6;
				jforce = rabbit->GetMass() * forward / (1 / 60.0);
				bforce = rabbit->GetMass()*backward / (1 / 60.0);
				jforce /= 6;
				bforce /= 6;
				backward = 0.0f;
				forward = 0.0f;
			}

			break;
		case GLFW_KEY_S:
			backward = 0.0f;
			forward = 0.0f;
			break;
		case GLFW_KEY_E:
			jumpstep = 6;
			break;
		}
	}

	void Step(Settings* settings)
	{
		//run the default physics and rendering
		Test::Step(settings);

		//properties
		angle = rabbit->GetAngle();
		angleVelocity = rabbit->GetAngularVelocity();
		speed = rabbit->GetLinearVelocity();
		jumpAvail = rabbit->IsAwake();

		//show some text in the main screen
		g_debugDraw.DrawString(5, m_textLine, "Bunny vs Rabbit");
		m_textLine += 15;
		g_debugDraw.DrawString(5, m_textLine, "Angle : %.3f  Angular velocity: %.3f", angle*RADTODEG, angleVelocity*RADTODEG);
		m_textLine += 15;
		g_debugDraw.DrawString(5, m_textLine, "speed x : %.3f, y : %.3f", speed.x, speed.y);
		m_textLine += 15;
		g_debugDraw.DrawString(5, m_textLine, "front : %.3f, back : %.3f", forward, backward);
		m_textLine += 15;
		if (jumpAvail) {
			g_debugDraw.DrawString(5, m_textLine, "unfixed");
			m_textLine += 15;
		}
		else {
			g_debugDraw.DrawString(5, m_textLine, "fixed");
			m_textLine += 15;
		}

		if (jumpstep>0)
		{
			rabbit->ApplyForce(b2Vec2(0, bforce), rabbit->GetWorldPoint(b2Vec2(10.0f, 1.0f)), true);
			rabbit->ApplyForce(b2Vec2(0, jforce), rabbit->GetWorldPoint(b2Vec2(-10.0f, 1.0f)), true);

			rabbit->ApplyForce(b2Vec2(jforce, 0), rabbit->GetWorldCenter(), true);
			jumpstep--;
		}

		//check angle
		if (!jumpAvail && rabbit->GetAngularVelocity() == 0)
		{
			int a = angle * RADTODEG;
			a %= 360;
			if (a < 0) a += 360;
			g_debugDraw.DrawString(5, m_textLine, "angle %d", a);

			
			if (100 < a && a < 280)
			{
				rabbit->SetTransform(beforePosition, 0.0);
			}
			
		}
	}



	static Test* Create()
	{
		return new Bunny;
	}

	b2Body* rabbit;
	float32 angle;
	float32 angleVelocity;
	b2Vec2 speed;
	bool jumpAvail;
	float32 forward;
	float32 backward;
	float32 jforce;
	float32 bforce;
	int jumpstep;
};

#endif