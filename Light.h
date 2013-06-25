#include <GL/glut.h>

using namespace std;

class Light
{
    private:
        float _position[4];
        float _diffuse[4];
        float _ambiant[4];
        float _specular[4];
        float _shininess;
        bool _enabled;

    public:
        Light();
        Light(float* &, float* & = 0, float* & = 0, float* & = 0, float = 0);
        ~Light();

	const float* getAmbiant() const	{return _ambiant;}
	const float* getDiffuse() const	{return _diffuse;}
	const float* getPosition() const {return _position;}
	const float getShininess() const {return _shininess;}
	void setShininess(const float shininess){_shininess = shininess;}
	const float* getSpecular() const {return _specular;}
	const bool isEnabled() const {return _enabled;}
	void setEnabled();

};
