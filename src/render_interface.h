#pragma once
#include "assets.h"
#include "shader_header.h"
#include "Alan_lib.h"
// #############################################################################
//                           Render Constants
// #############################################################################

int RENDER_OPTION_FLIP_X = BIT(0);
int RENDER_OPTION_FLIP_Y = BIT(1);

// #############################################################################
//                           Renderer Structs
// #############################################################################

enum Layer
{
  LAYER_GAME,
  LAYER_UI,
  LAYER_COUNT
};


struct OrthographicCamera2D
{
  float zoom = 1.0f;
  Vec2 dimensions;
  Vec2 position;
};

struct DrawData
{
  Material material = {};
  int animationIdx;
  int renderOptions;
  float layer = 0.0f;
};


struct Glyph
{
  Vec2 offset;
  Vec2 advance;
  IVec2 textureCoords;
  IVec2 size;
};

struct RenderData
{
    OrthographicCamera2D gameCamera;
    OrthographicCamera2D uiCamera;
    

    Glyph glyphs[127];

    int transformCount;
    Array<Material, 1000> materials;
    Array<Transform, 1000> transforms;
};

// #############################################################################
//                           Renderer Globals
// #############################################################################
static RenderData* renderData;

// #############################################################################
//                           Renderer Untility
// #############################################################################
IVec2 screen_to_world(IVec2 screenPos)
{
  OrthographicCamera2D camera = renderData->gameCamera;

  int xPos = (float)screenPos.x / 
             (float)input->screenSize.x * 
             camera.dimensions.x; // [0; dimensions.x]

  // Offset using dimensions and position
  xPos += -camera.dimensions.x / 2.0f + camera.position.x;

  int yPos = (float)screenPos.y / 
             (float)input->screenSize.y * 
             camera.dimensions.y; // [0; dimensions.y]

  // Offset using dimensions and position
  yPos += camera.dimensions.y / 2.0f + camera.position.y;

  return {xPos, yPos};
}



int get_material_idx(Material material = {})
{
  // Convert from SRGB to linear color space, to be used in the shader, poggies
  material.color.r = powf(material.color.r, 2.2f);
  material.color.g = powf(material.color.g, 2.2f);
  material.color.b = powf(material.color.b, 2.2f);
  material.color.a = powf(material.color.a, 2.2f);

  for(int materialIdx = 0; materialIdx < renderData->materials.count; materialIdx++)
  {
    if(renderData->materials[materialIdx] == material)
    {
      return materialIdx;
    }
  }

  return renderData->materials.add(material);
}
float get_layer(Layer layer, float subLayer = 0.0f)
{
  float floatLayer = (float)layer;
  float layerStep = 1.0f / (float)LAYER_COUNT;
  float result = layerStep * floatLayer + subLayer / 1000.0f;
  return result;
}
Transform get_transform(SpriteID spriteID, Vec2 pos, Vec2 size = {}, DrawData drawData = {})
{
  Sprite sprite = get_sprite(spriteID);
  size = size? size: vec_2(sprite.size);

  Transform transform = {};
  transform.materialIdx = get_material_idx(drawData.material);
  transform.pos = pos - size / 2.0f;
  transform.size = size;
  transform.atlasOffset = sprite.atlasOffset;
  // For Anmations, this is a multiple of the sprites size,
  // based on the animationIdx
  transform.atlasOffset.x += drawData.animationIdx * sprite.size.x;
  transform.spriteSize = sprite.size;
  transform.renderOptions = drawData.renderOptions;
  transform.layer = drawData.layer;

  return transform;
}

// #############################################################################
//                           Renderer Functions
// #############################################################################

// void draw_sprite(SpriteID spriteID, Vec2 pos)
// {
//     Sprite sprite = get_sprite(spriteID);
    
//     Transform transform = {};
//     transform.pos = pos - vec_2(sprite.size) / 2.0f;
//     transform.size = vec_2(sprite.size);
//     transform.atlasOffset = sprite.atlasOffset;
//     transform.spriteSize = sprite.size;

//     renderData->transforms[renderData->transformCount++] = transform;

// }
void draw_quad(Transform transform)
{
  renderData->transforms.add(transform);
}

void draw_quad(Vec2 pos, Vec2 size, DrawData drawData = {})
{
  Transform transform = get_transform(SPRITE_WHITE, pos, size, drawData);
  renderData->transforms.add(transform);
}

void draw_sprite(SpriteID spriteID, Vec2 pos, DrawData drawData = {})
{
  Transform transform = get_transform(spriteID, pos, {}, drawData);
  renderData->transforms.add(transform);
}

void draw_sprite(SpriteID spriteID, IVec2 pos, DrawData drawData = {})
{
  draw_sprite(spriteID, vec_2(pos), drawData);
}