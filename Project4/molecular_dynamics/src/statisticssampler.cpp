#include "system.h"
#include "statisticssampler.h"
#include "lennardjones.h"
#include <iostream>
#include <iomanip>

using std::ofstream; using std::cout; using std::endl;

StatisticsSampler::StatisticsSampler()
{

}

void StatisticsSampler::saveToFile(System &system)
{
    // Save the statistical properties for each timestep for plotting etc.
    // First, open the file if it's not open already
    if(!m_file.good()) {
        m_file.open("statistics.txt", ofstream::out);
        // If it's still not open, something bad happened...
        if(!m_file.good()) {
            cout << "Error, could not open statistics.txt" << endl;
            exit(1);
        }
    }

    // Print out values here
    m_file << std::setw(10) <<system.time() <<
              std::setw(10) << m_kineticEnergy<<
              std::setw(10) << m_potentialEnergy <<
              std::setw(12) << m_temperature<<
              std::setw(10)<<m_diffusion_coeff<<endl;
}

void StatisticsSampler::sample(System &system)
{
    // Here you should measure different kinds of statistical properties and save it to a file.
    sampleKineticEnergy(system);
    samplePotentialEnergy(system);
    sampleTemperature(system);
    sampleDiffusionCoeff(system);
    sampleDensity(system);
    saveToFile(system);
}

void StatisticsSampler::sampleKineticEnergy(System &system)
{
    m_kineticEnergy = 0.; // Remember to reset the value from the previous timestep
    for(Atom *atom : system.atoms()) {
        m_kineticEnergy += 0.5*atom->mass()*atom->velocity.lengthSquared();
    }
}

void StatisticsSampler::samplePotentialEnergy(System &system)
{
    m_potentialEnergy = system.potential().potentialEnergy();
}

void StatisticsSampler::sampleTemperature(System &system)
{
    // Hint: reuse the kinetic energy that we already calculated
    m_temperature = (2./3.)*m_kineticEnergy/system.num_atoms();   //num_atoms() is not working properly: says its 0!
    //cout <<"num_atoms= " << system.num_atoms() << endl;
}

void StatisticsSampler::sampleDensity(System &system)
{

}

void StatisticsSampler::sampleDiffusionCoeff(System &system){
    double displacements_sqrd_sum = 0.0;  //reset displacements sum
    for(Atom *atom : system.atoms()) {
        vec3 total_displacement;
        for(int j=0;j<3;j++){
            total_displacement[j] = abs((atom->initial_position(j) - atom->position[j]) + atom->num_bndry_crossings[j]*system.systemSize(j));
            //total_displacement[j] = (atom->position[j] - atom->initial_position(j)) + abs(atom->num_bndry_crossings[j])*system.systemSize(j);
            //MAKE SURE THAT THIS IS RIGHT!, CHECK MATHEMATICALLY!
        }
        //takes into account displacement within 1 cell plus displacement due to crossing boundaries into neighboring image cells (PBCs)
        double total_displacement_sqrd = total_displacement.lengthSquared();
        displacements_sqrd_sum += total_displacement_sqrd;
        }
    m_diffusion_coeff = displacements_sqrd_sum/(6*system.time()*system.num_atoms());  //Einstein relation: D = (mean sqr displacement)/6t
}


