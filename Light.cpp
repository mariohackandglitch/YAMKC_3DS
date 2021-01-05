#include "Light.hpp"

Color Light::GlobalAmbientColor = Color(1.f, 1.f, 1.f);

Light::Light(GLenum glLight)
{
    this->light = glLight;
    position = Vector3(0.f, 0.f, 1.f);
    enabled = true;
    type = LightType::DIRECTIONAL;

    AmbientColor = Color(1.f, 1.f, 1.f);
    DiffuseColor = Color(1.f, 1.f, 1.f);
    SpecularColor = Color(0.f, 0.f, 0.f);
}

Light::~Light()
{
    glDisable(light);
}

void Light::Enable()
{
    enabled = true;
}

void Light::Disable()
{
    enabled = false;
}

void Light::SetType(LightType t)
{
    this->type = t;
}

void Light::SetPosition(const Vector3& pos)
{
    this->position = pos;
}

void Light::SetColor(ColorType type, const Color& color)
{
    switch (type)
    {
    case Light::ColorType::AMBIENT:
        AmbientColor = color;
        break;
    case Light::ColorType::DIFFUSE:
        DiffuseColor = color;
        break;
    case Light::ColorType::SPECULAR:
        SpecularColor = color;
        break;
    default:
        break;
    }
}

void Light::GlobalEnable()
{
    GLfloat A[] = { GlobalAmbientColor.r, GlobalAmbientColor.g, GlobalAmbientColor.b, GlobalAmbientColor.a};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, A);

    glEnable(GL_LIGHTING);
}

void Light::GlobalDisable()
{
    glDisable(GL_LIGHTING);
}

void Light::Update()
{
    if (enabled)
        glEnable(light);
    else
        glDisable(light);

    GLfloat args[4] = { position.x, position.y, position.z, (type == LightType::DIRECTIONAL) ? 0.f : 1.f };
    glLightfv(light, GL_POSITION, args);
    args[0] = AmbientColor.r; args[1] = AmbientColor.g; args[2] = AmbientColor.b; args[3] = AmbientColor.a;
    glLightfv(light, GL_AMBIENT, args);
    args[0] = DiffuseColor.r; args[1] = DiffuseColor.g; args[2] = DiffuseColor.b; args[3] = DiffuseColor.a;
    glLightfv(light, GL_DIFFUSE, args);
    args[0] = SpecularColor.r; args[1] = SpecularColor.g; args[2] = SpecularColor.b; args[3] = SpecularColor.a;
    glLightfv(light, GL_SPECULAR, args);

}
