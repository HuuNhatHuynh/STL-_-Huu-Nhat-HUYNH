#include "stl_loader.hpp"
#include <math.h>

//Les structures de donnees (Vertex, Normal, Facet, Mesh) definies dans le fichier stl_loader.h

//créez une fonction de template pour utiliser avec 2 types de données différents: Normal et Vertex
template<typename T> T rotate(T v, float theta)
{
    T ans;

    //nous devons changer les coordonnees par une matrice tournant dans la direction x avec les anciennes coordonnees
    ans.x = v.x;
    ans.y = v.y * cos(theta) - v.z * sin(theta);
    ans.z = v.y * sin(theta) + v.z * cos(theta);

    return ans;
}

//fonction pour faire tourner le maillage dans la direction x d'un angle theta
Mesh stl_rotate(Mesh mesh_input, float theta)
{
    Mesh mesh_output;

    std::vector<Facet> Facet_input = mesh_input.facets;
    std::vector<Facet> Facet_output;

    unsigned int n = Facet_input.size();

    for (unsigned int i = 0; i < n; i++)
    {
        Facet f;

        //tourer le vecteur normal d'un angle theta
        f.normal = rotate(Facet_input[i].normal, theta);

        //tourner les sommets du triangle d'un angle theta
        f.vertices[0] = rotate(Facet_input[i].vertices[0], theta);
        f.vertices[1] = rotate(Facet_input[i].vertices[1], theta);
        f.vertices[2] = rotate(Facet_input[i].vertices[2], theta);

        Facet_output.push_back(f);
    }

    mesh_output.facets = Facet_output;

    return mesh_output;
}
