#version 150

vec4 getTexColor(vec2 off);

vec4 calcColor();

bool equalFloat(float a, float b);

bool eqCol(vec4 color1, vec4 color2);

bool neqCol(vec4 color1, vec4 color2);


in vec2 _texCoord;

out vec4 FragColor;

vec2 unitPixel;
vec4 black = vec4(0.0,0.0,0.0,1.0);
vec4 sand_color = vec4(0.8,0.8,0.0,1.0);
vec4 wall_color = vec4(0.15,0.80,0.2,1.0);

uniform sampler2D texture_u;//  glBindTexture(GL_TEXTURE_2D, textures[src])

uniform vec2 mouse_pos_u;

uniform int mouse_leftButton_u;

uniform int sand_mode_u;

uniform int wall_mode_u;

uniform int eraser_mode_u;


uniform float width;

uniform float height;


vec2 up    = vec2(0.0, 1.0);

vec2 down  = vec2(0.0,-1.0);

vec2 left  = vec2(-1.0,0.0);

vec2 right = vec2(1.0, 0.0);

vec2 here  = vec2(0.0, 0.0);

vec4 upColor;

vec4 downColor;

vec4 hereColor;

vec4 leftColor;

vec4 leftleftColor;

vec4 leftleftleftColor;

vec4 rightColor;

vec4 rightrightColor;

vec4 downleftColor;

vec4 downleftleftColor;

vec4 downrightColor;

vec4 downrightrightColor;

vec4 upleftColor;

vec4 upleftleftColor;

vec4 uprightColor;

vec2 pos;

//circle arround the mouse
float isIn;
//if the mouse is clicked, draws particles arround the circle
vec4 clicked_circle; // NEED more details

void main()
{
    unitPixel = vec2(1.0,1.0)/vec2(width,height);
    pos = gl_FragCoord.xy/vec2(width,height);
    isIn = step(sqrt(pow((mouse_pos_u.x-pos.x)*width,2)+pow((mouse_pos_u.y-pos.y)*height,2)),width*0.05);
    clicked_circle = isIn*mouse_leftButton_u*(sand_mode_u*sand_color + wall_mode_u*wall_color + eraser_mode_u*black); // NEED more details


    upColor = getTexColor(up);

    downColor = getTexColor(down);

    hereColor = getTexColor(here);

    leftColor = getTexColor(left);

    leftleftColor = getTexColor(left+left);

    leftleftleftColor = getTexColor(left+left+left);

    rightColor = getTexColor(right);

    rightrightColor = getTexColor(right+right);

    downleftColor = getTexColor(down+left);

    downleftleftColor = getTexColor(down+left+left);

    downrightColor = getTexColor(down+right);

    downrightrightColor = getTexColor(down+right+right);

    upleftColor = getTexColor(up+left);

    upleftleftColor = getTexColor(up+left+left);

    uprightColor = getTexColor(up+right);

    vec4 res = calcColor(); // calccolor is the most important func

    FragColor = res; // is the thing which will be shown on the screen
    // FragColor = texture(texture_u, _texCoord);
    // color_out = res;
}

bool equalFloat(float a, float b){
    return abs(a-b) < 0.01;
}

bool eqCol(vec4 color1, vec4 color2)
{
    return equalFloat(color1.x,color2.x) && equalFloat(color1.y,color2.y) && equalFloat(color1.z,color2.z);
}

bool neqCol(vec4 color1, vec4 color2)
{
    return !eqCol(color1,color2);
}

vec4 calcColor(){

    if(eraser_mode_u * isIn * mouse_leftButton_u == 1) return black;
    if((hereColor == black) && (upColor == black) && (downColor == black) && (rightColor == black) && (leftColor == black)) return clicked_circle;

    /* wall rules */
    if(eqCol(wall_color,hereColor)) return hereColor;
    /* end of wall rules */

    /*sand rules*/
    //dont falling from canvas
    if(pos.y <= unitPixel.y && eqCol(hereColor,sand_color)) return hereColor;
    if(pos.x <= unitPixel.x && eqCol(upColor,sand_color)) return upColor;
    if(pos.x <= unitPixel.x && eqCol(uprightColor,sand_color)) return uprightColor;
    if(pos.x >= (unitPixel.x * width) && eqCol(upColor,sand_color)) return upColor;
    if(pos.x >= (unitPixel.x * (width-1)) && eqCol(upleftColor,sand_color)) return upleftColor;

   //falling
    if(eqCol(hereColor,black) && eqCol(upColor,sand_color)) return upColor;
    if(eqCol(hereColor,sand_color) && eqCol(downColor,black)) return black;

    //if(eqCol(hereColor,black) && eqCol(rightColor,sand_color)&& eqCol(rightrightColor, sand_color)&& eqCol(uprightColor, sand_color)) return sand_color ;
    if(eqCol(hereColor,sand_color) && eqCol(leftColor,black)&& eqCol(rightrightColor, sand_color)&& eqCol(downleftColor, black)) return uprightColor ; //for the left

  //  if(eqCol(hereColor,sand_color) && eqCol(leftColor,black)&& eqCol(rightColor, black)&& eqCol(downColor, sand_color)&& eqCol(downleftColor, sand_color)&& eqCol(downrightColor, sand_color) && eqCol(upColor, black)) return black ; //for the left



    if(eqCol(hereColor,sand_color) && eqCol(rightColor,black)&& eqCol(leftleftColor, sand_color)&& eqCol(downrightColor, black)) return upleftColor ; // for the right

 //   if(eqCol(hereColor,sand_color) && eqCol(rightColor,black)&& eqCol(leftleftColor, sand_color)&& eqCol(downrightColor, black)) return upleftColor ; // for the right




    //TODO there's one border case left on sliding behaviour
    //stacking up (on sand or on wall) or sliding if posible
    if(eqCol(hereColor,sand_color) && (eqCol(downColor,sand_color) || eqCol(downColor,wall_color)))

       // if(eqCol(downleftColor,sand_color) && eqCol(rightColor,black) && eqCol(downrightColor,black) && eqCol(rightrightColor,black) && eqCol(upColor,black) && eqCol(uprightColor,black))  //fall to the right
         //   return black;

       // else if(eqCol(downrightColor,sand_color) && eqCol(leftColor,black) && eqCol(downleftColor,black) && eqCol(leftleftColor,black) && eqCol(upColor,black) && eqCol(upleftColor,black))  //fall to the left
          //  return black;

      //  else if((leftleftColor == black) && (downleftColor == black) && (leftColor == black) && (upleftColor == black) && (upColor == black) && (uprightColor == black) && (rightColor == black) && (rightrightColor == black) && (downrightColor == black)) //fal to one of both sides (supose the right one)
          //  return black;

            return hereColor;

    //sliding coloring rules
    if(eqCol(hereColor,black) && eqCol(leftColor,sand_color) && eqCol(downleftColor,sand_color) && eqCol(downleftleftColor,sand_color) && eqCol(rightColor,black) && eqCol(upColor,black) && eqCol(upleftColor,black) && eqCol(downColor,black))
        return leftColor;
    if(eqCol(hereColor,black) && eqCol(rightColor,sand_color) && eqCol(downrightColor,sand_color) && eqCol(downrightrightColor,sand_color) && eqCol(leftColor,black) && eqCol(upColor,black) && eqCol(uprightColor,black) && eqCol(downColor,black))
        return rightColor;
    if(eqCol(hereColor,black) && eqCol(downColor,black) && eqCol(rightColor,black) && eqCol(upColor,black) && eqCol(leftColor,sand_color) && eqCol(downleftColor,sand_color) && eqCol(upleftColor,black) && eqCol(upleftleftColor,black) && eqCol(leftleftColor,black) && eqCol(downleftleftColor,black) && eqCol(leftleftleftColor,black))
        return leftColor;
    // end of sand rules

    return clicked_circle;
}
vec4 getTexColor(vec2 off)
{
    if(_texCoord.y + unitPixel.y > height+1) return black; //

    if(_texCoord.y + unitPixel.y < -1) return black;     /// CHEcking bounds

    return texture(texture_u, vec2(_texCoord.x + unitPixel.x*off.x,_texCoord.y + unitPixel.y*off.y)); // Getting from teture_u  coords of the pixel
                                                                                                    // _texCoord is a coord of current pixel
                                                                                                    //unitPixel is like a size of one pixel on the screen
                                                                                                    //

}
