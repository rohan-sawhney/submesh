#include "Mesh.h"
#include "MeshIO.h"
#include <stack>
#include <list>

Mesh::Mesh()
{
    
}

Mesh::Mesh(const Mesh& mesh)
{
    *this = mesh;
}

bool Mesh::read(const std::string& fileName, const int mode)
{
    std::ifstream in(fileName.c_str());

    if (!in.is_open()) {
        std::cerr << "Error: Could not open file for reading" << std::endl;
    }
    
    bool readSuccessful = false;
    if ((readSuccessful = MeshIO::read(in, *this, mode))) {
        normalize();
    }
    
    return readSuccessful;
}

bool Mesh::write(const std::string& fileName) const
{
    std::ofstream out(fileName.c_str());
    
    if (!out.is_open()) {
        std::cerr << "Error: Could not open file for writing" << std::endl;
    }
    
    MeshIO::write(out, *this);
    
    return false;
}

std::vector<Mesh> Mesh::generateSubmeshesEdgeApproach()
{
    std::vector<Mesh> meshes;
    
    int seen = 0;
    int meshSize = 0;
    while (seen != vertices.size()) {
        Mesh mesh;
        meshes.push_back(mesh);
        
        std::stack<HalfEdgeIter> stack;
        
        VertexIter v;
        for (v = vertices.begin(); v != vertices.end(); v++) {
            if (!v->seen) {
                v->seen = true;
                seen ++;
                break;
            }
        }
        meshes[meshSize].vertices.push_back(*v);
        
        do {
            HalfEdgeIter he = v->he;
            do {
                if (!he->seen) {
                    he->seen = true;
                    stack.push(he);
                    meshes[meshSize].halfEdges.push_back(*he);
                    
                    if (!he->edge->seen) {
                        he->edge->seen = true;
                        meshes[meshSize].edges.push_back(*he->edge);
                    }
                    
                    if (!he->face->seen) {
                        he->face->seen = true;
                        meshes[meshSize].faces.push_back(*he->face);
                    }
                }
                
                he = he->flip->next;
            } while (he != v->he);
            
            v = stack.top()->flip->vertex;
            if (!v->seen) {
                v->seen = true;
                seen ++;
                meshes[meshSize].vertices.push_back(*v);
            }
            
            stack.pop();
        } while (!stack.empty());
        
        meshSize ++;
    }
    
    return meshes;
}

std::vector<Mesh> Mesh::generateSubmeshesFaceApproach()
{
    std::vector<Mesh> meshes;
    
    int seen = 0;
    int meshSize = 0;
    while (seen != facePointers.size()) {
        Mesh mesh;
        meshes.push_back(mesh);
        
        std::stack<Face*> stack;
        Face *f = NULL;
        for (int i = 0; i < (int)facePointers.size(); i++) {
            if (!facePointers[i]->seen) {
                f = facePointers[i];
                f->seen = true;
                stack.push(f);
                seen ++;
                break;
            }
        }
        
        if (f) {
            do {
                f = stack.top();
                stack.pop();
                
                for (int i = 0; i < (int)f->adjFaces.size(); i++) {
                    if (!f->adjFaces[i]->seen) {
                        f->adjFaces[i]->seen = true;
                        stack.push(f->adjFaces[i]);
                        seen ++;
                    }
                }
                
                meshes[meshSize].facePointers.push_back(f);
            } while (!stack.empty());
        }
        
        std::unordered_map<int, int> indices;
        for (int i = 0; i < (int)meshes[meshSize].facePointers.size(); i++) {
            Face *f = meshes[meshSize].facePointers[i];
            
            if (indices.find(f->a) == indices.end()) {
                meshes[meshSize].vertices.push_back(vertices[f->a]);
                indices[f->a] = (int)meshes[meshSize].vertices.size()-1;
            }
            f->a = indices[f->a];
            
            if (indices.find(f->b) == indices.end()) {
                meshes[meshSize].vertices.push_back(vertices[f->b]);
                indices[f->b] = (int)meshes[meshSize].vertices.size()-1;
            }
            f->b = indices[f->b];
            
            if (indices.find(f->c) == indices.end()) {
                meshes[meshSize].vertices.push_back(vertices[f->c]);
                indices[f->c] = (int)meshes[meshSize].vertices.size()-1;
            }
            f->c = indices[f->c];
        }
        
        meshSize ++;
    }
    
    return meshes;
}

void Mesh::normalize()
{
    // compute center of mass
    Eigen::Vector3d cm = Eigen::Vector3d::Zero();
    for (VertexCIter v = vertices.begin(); v != vertices.end(); v++) {
        cm += v->position;
    }
    cm /= (double)vertices.size();
    
    // translate to origin
    for (VertexIter v = vertices.begin(); v != vertices.end(); v++) {
        v->position -= cm;
    }
    
    // determine radius
    double rMax = 0;
    for (VertexCIter v = vertices.begin(); v != vertices.end(); v++) {
        rMax = std::max(rMax, v->position.norm());
    }
    
    // rescale to unit sphere
    for (VertexIter v = vertices.begin(); v != vertices.end(); v++) {
        v->position /= rMax;
    }
}
