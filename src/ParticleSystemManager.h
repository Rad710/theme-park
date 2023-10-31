#pragma once

#include <GL/GLU.h>
#include "Utilities/Pnt3f.H"
#include <vector>
#include <ctime>
#include <random>

using namespace std;

// define the properties of the particles
struct Particle 
{
    Pnt3f position;
    Pnt3f velocity;
    float size;
};


class ParticleSystem 
{
public:
    ParticleSystem() {}

    //default constructor
    ParticleSystem(int numParticles, float lifeTime, Pnt3f dir) : halfLife(lifeTime), initialLifeTime(lifeTime), direction(dir)
    {
        particles.resize(numParticles);
    }

    //move constructor
    ParticleSystem(ParticleSystem&& other) noexcept
        : particles(move(other.particles)), halfLife(other.halfLife), initialLifeTime(other.initialLifeTime),
        direction(other.direction)
    {
        // clear data
        other.halfLife = 0.0f;
        other.initialLifeTime = 0.0f;
        other.direction = Pnt3f();
    }

    // move assignment operator
    ParticleSystem& operator=(ParticleSystem&& other) noexcept 
    {
        if (this != &other) 
        {
            particles = move(other.particles);

            halfLife = other.halfLife;
            initialLifeTime = other.initialLifeTime;
            direction = other.direction;

            //clear data
            other.halfLife = 0.0f;
            other.initialLifeTime = 0.0f;
            other.direction = Pnt3f();

        }
        return *this;
    }

    //set all particles to initial position
    void setPosition(Pnt3f position, Pnt3f orientation) 
    {
        srand(time(NULL));

        for (auto& particle : particles) 
        {
            particle.position = position;
            particle.velocity = orientation;
            particle.size = 0.1f;
        }
    }

    void update(float deltaTime) {
        //randomness to the movement
        float randomness = 0.2f;

        halfLife -= deltaTime; // decay 

        if (halfLife <= initialLifeTime * 0.9f) //dont spread too much after a while
        {
            randomness = 0.0f;
        }

        for (auto& particle : particles)
        {
            Pnt3f randomOffset(
                static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * randomness - randomness / 2,
                static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * randomness - randomness / 2,
                static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * randomness - randomness / 2
            );
            if (halfLife <= initialLifeTime * 0.7f) //after a while go up
            {
                particle.velocity = direction;
            }
            else
            {
                particle.velocity = particle.velocity + randomOffset;
            }
            particle.position = particle.position + particle.velocity * deltaTime;
        }
    }

    void render()
    {
        //each particle is a point
        glBegin(GL_POINTS);
        for (auto& particle : particles) 
        {
            glNormal3f(particle.velocity.x, particle.velocity.y, particle.velocity.z);
            glColor3f(0.3f, 0.3f, 0.3f);
            glPointSize(particle.size);
            glVertex3f(particle.position.x, particle.position.y, particle.position.z);
        }
        glEnd();
    }

    bool isDead() const //check if system decayed
    {
        return halfLife <= 0.0f;
    }

public:
    vector<Particle> particles; //system of particles
    float halfLife;
    float initialLifeTime;

    Pnt3f direction;
};


static const int NUM_PARTICLE_SYSTEMS = 10;

//to dynamically control particle systems and make up smoke
class ParticleSystemManager 
{
public:
    void update(float deltaTime, const Pnt3f& position, const Pnt3f& orientation) 
    {
        for (auto& particleSystem : particleSystems) 
        {
            particleSystem.update(deltaTime);
        }

        // remove dead particle systems
        for (int i = 0; i < particleSystems.size(); i++)
        {
            if (particleSystems[i].isDead())
            {
                particleSystems.erase(particleSystems.begin() + i);
            }
        }

        // renew particle systems
        if (particleSystems.size() < NUM_PARTICLE_SYSTEMS) 
        {
            ParticleSystem newParticleSystem(1000, 70.0f, direction);
            newParticleSystem.setPosition(position, orientation);
            particleSystems.push_back(move(newParticleSystem));
        }
    }


    void render() 
    {
        for (auto& particleSystem : particleSystems) 
        {
            particleSystem.render();
        }
    }

    void clear()
    {
        particleSystems.clear();
    }

    void setDirection(Pnt3f dir)
    {
        direction = dir;
    }

private:
    vector<ParticleSystem> particleSystems;

    Pnt3f direction;
};
