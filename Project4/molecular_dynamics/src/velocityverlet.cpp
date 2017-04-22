#include "velocityverlet.h"
#include "system.h"
#include "atom.h"

void VelocityVerlet::integrate(System &system, double dt)
{
    if(m_firstStep) {
        system.calculateForces();
        m_firstStep = false;
    }

    for(Atom *atom : system.atoms()) {
        //this operates on the vectors directly using vec3 class
        atom->velocity += atom->force*0.5*dt/atom->mass();
        atom->position += atom->velocity*dt;  //NOTE: since v is computed 1st, this is actually v = vt+0.5at^2 since v =  v+0.5at!!
       //NOTE: there was a mistake in the original cloned version from Github: had velocity/m for position.
    }

    system.applyPeriodicBoundaryConditions();
    system.calculateForces(); // New positions, recompute forces

    for(Atom *atom : system.atoms()) {
        atom->velocity += atom->force*0.5*dt/atom->mass();  //calculate new velocities
    }
}