/*===========================================================================
    Çàãîëîâîê: TShaderParser.cpp
    Àâòîğ: Evill games, Kashey
    Èíôîğìàöèÿ: .shader file parser
	TODO:
		- we spend a lot of time in stricmp
		- use a hash table to parse the commands and minimize stricmps
		- generate warning if any stage after the first stage does not have a
		  blendFunc specified.
===========================================================================*/

#include "stdafx.h"
#include "ShaderParser.h"



void DLLTYPE ShadersInit()
{					 
  CShaderParser StartParser(0);
}

void DLLTYPE ShadersPass()
{					 
  CShaderParser StartParser(0);
  StartParser.StartParseFile("common.shader");
  //Print2Console(SYS_SHADERS,"FoundShader:%s",StartParser.GetFirstShader());
  while(1)
  {
	  StartParser.ParseShader();
	  LPCSTR SName=StartParser.GetNextShader();
	  if(!SName)break;
	 // Print2Console(SYS_SHADERS,"FoundShader:%s",SName);
  }
 //Print2Console(SYS_SHADERS,"END");
}



/*---------------------------------------------------------------------------
    Ìåòîäû:
---------------------------------------------------------------------------*/

CShaderParser::_MYBSPDIVIDOR  CShaderParser::BSPFor;
bool                          CShaderParser::BSPFuncInited=false;

#define SDR_skyParms							1
#define SDR_cull								2
#define SDR_terrainfog							3
#define SDR_deformVertexes						4
#define SDR_fogParams							5
#define SDR_terrainfogparams					6
#define SDR_nopicmip							7
#define SDR_nobright							8
#define SDR_nomipmaps							9
#define SDR_polygonOffset						10
#define SDR_portal								11
#define SDR_sort								12
#define SDR_tessSize							13
#define SDR_q3map_backshader					14
#define SDR_q3map_globaltexture					15
#define SDR_q3map_sun							16
#define SDR_q3map_surfaceLight					17
#define SDR_q3map_lightImage					18
#define SDR_q3map_lightSubdivide				19
#define SDR_surfaceParm							20
#define SDR_qer_editorimage						21
#define SDR_qer_nocarve							22
#define SDR_qer_trans							23
#define SDR_backsided							24
#define SDR_cloudParms							25
#define SDR_entityMergable						26
#define SDR_fogGen								27
#define SDR_fogOnly								28
#define SDR_light								29
#define SDR_lightning							30
#define SDR_q3map_backsplash					31
#define SDR_q3map_flare							32
#define SDR_sky									33
#define SDR_terrain								34
#define SDR_stenciled_flat_mirror				35
#define SDR_surfaceColor						36
#define SDR_spriteParam							37
#define SDR_include_effect 						38

#define SDR_map								    40
#define SDR_clampMap							41
#define SDR_animMap								42
#define SDR_blendFunc							43
#define SDR_rgbGen								44
#define SDR_alphaGen							45
#define SDR_tcGen								46
#define SDR_tcMod								47
#define SDR_depthFunc							48
#define SDR_depthMask							49
#define SDR_detail								50
#define SDR_alphaFunc							51


void    CShaderParser::ConstructBSP()
{
  GStringBase Base;
#define ADDBSP(b,a) Base=a;Base.MakeLower();BSPFor.PushAs(b,Base.v_str(),(DWORD)strlen(a))
  ADDBSP(SDR_skyParms              ,"skyParms"			   );
  ADDBSP(SDR_cull                  ,"cull"				   );
  ADDBSP(SDR_terrainfog            ,"terrainfog"		   );
  ADDBSP(SDR_deformVertexes        ,"deformVertexes"	   );
  ADDBSP(SDR_fogParams             ,"fogParams"		       );
  ADDBSP(SDR_terrainfogparams      ,"terrainfogparams"	   );
  ADDBSP(SDR_nopicmip              ,"nopicmip"			   );
  ADDBSP(SDR_nopicmip              ,"nopicmip"			   );
  ADDBSP(SDR_nomipmaps             ,"nomipmaps"		       );
  ADDBSP(SDR_polygonOffset         ,"polygonOffset"	       );
  ADDBSP(SDR_portal                ,"portal"			   );
  ADDBSP(SDR_sort                  ,"sort"				   );
  ADDBSP(SDR_tessSize              ,"tessSize"			   );
  ADDBSP(SDR_q3map_backshader      ,"q3map_backshader"	   );
  ADDBSP(SDR_q3map_globaltexture   ,"q3map_globaltexture"  );
  ADDBSP(SDR_q3map_sun             ,"q3map_sun"		       );
  ADDBSP(SDR_q3map_surfaceLight    ,"q3map_surfaceLight"   );
  ADDBSP(SDR_q3map_lightImage      ,"q3map_lightImage"	   );
  ADDBSP(SDR_q3map_lightSubdivide  ,"q3map_lightSubdivide" );
  ADDBSP(SDR_surfaceParm           ,"surfaceParm"		   );
  ADDBSP(SDR_qer_editorimage       ,"qer_editorimage"	   );
  ADDBSP(SDR_qer_nocarve           ,"qer_nocarve"		   );
  ADDBSP(SDR_qer_trans             ,"qer_trans"		       );
  ADDBSP(SDR_backsided             ,"backsided"		       );
  ADDBSP(SDR_cloudParms            ,"cloudParms"		   );
  ADDBSP(SDR_entityMergable        ,"entityMergable"	   );
  ADDBSP(SDR_fogGen                ,"fogGen"			   );
  ADDBSP(SDR_fogOnly               ,"fogOnly"			   );
  ADDBSP(SDR_light                 ,"light"			       );
  ADDBSP(SDR_lightning             ,"lightning"		       );
  ADDBSP(SDR_q3map_backsplash      ,"q3map_backsplash"	   );
  ADDBSP(SDR_q3map_flare           ,"q3map_flare"		   );
  ADDBSP(SDR_sky                   ,"sky"				   );
  ADDBSP(SDR_terrain               ,"terrain"			   );
  ADDBSP(SDR_stenciled_flat_mirror ,"stenciled_flat_mirror");
  ADDBSP(SDR_surfaceColor          ,"surfaceColor"		   );
  ADDBSP(SDR_spriteParam           ,"spriteParam"		   );
  ADDBSP(SDR_include_effect        ,"include_effect"	   );


  ADDBSP(SDR_map                   ,"map"				   );
  ADDBSP(SDR_clampMap              ,"clampMap"			   );
  ADDBSP(SDR_animMap               ,"animMap"			   );
  ADDBSP(SDR_blendFunc             ,"blendFunc"			   );
  ADDBSP(SDR_rgbGen                ,"rgbGen"			   );
  ADDBSP(SDR_alphaGen              ,"alphaGen"			   );
  ADDBSP(SDR_tcGen                 ,"tcGen"				   );
  ADDBSP(SDR_tcMod                 ,"tcMod"				   );
  ADDBSP(SDR_depthFunc             ,"depthFunc"			   );
  ADDBSP(SDR_depthMask             ,"depthMask"			   );
  ADDBSP(SDR_detail                ,"detail"			   );
  ADDBSP(SDR_alphaFunc             ,"alphaFunc"			   );
  BSPFuncInited=true;
}


DWORD SYS_SHADERS=0;

CShaderParser::CShaderParser(int N):CParser()
{
 if(!BSPFuncInited)
	 ConstructBSP();
 if(SYS_SHADERS==0)
		RegisterNewConsoleClass(SYS_SHADERS,"SHADERS",-1);
}

CShaderParser::~CShaderParser()
{
}

//
// Get the first shader of the file (not really! see usage)
//
char *CShaderParser::GetFirstShader(void){
	errors=0;
	if(GetName())	return CParser::token;
	else 		    return NULL;
}
//
// Get the next shader of the file
//
char *CShaderParser::GetNextShader(void){
	errors=0;
	if(GetName())	return CParser::token;
	else         	return NULL;
}


//
// Skip the current shader
//
// FIXME: use == instead of strcmp since { and } are break characters
void CShaderParser::SkipShader(void){
	int depth=0;
	while(CParser::GetToken(true))
	{
		     if(IS_OPENBRACKET (CParser::token)) depth++;
		else if(IS_CLOSEBRACKET(CParser::token)) depth--;
		if(depth==0)break;
	}
}


//
// Parse the shader
//
void CShaderParser::ParseShader(void){

	Depth=1;
	int CurrentLine;
	GString Token;
	if(CParser::GetToken(true))
 	 if(!IS_OPENBRACKET(CParser::token)) {ShaderError("Warning, '{' expected");return;}
	CurrentLine=CParser::scriptline;
	while( CParser::GetToken(true) )
	{

		          if(IS_OPENBRACKET (CParser::token)) 
				  {Depth++;ParseLayer();continue;}
			 else if(IS_CLOSEBRACKET(CParser::token)) 
			      {Depth--;CloseLayer();if(!Depth)break;continue;}


		Token=CParser::token;
		Token.MakeLower();
		DWORD ShaderToken=BSPFor.GetIDFor(Token.c_str(),(DWORD)Token.length());
		switch(ShaderToken)
		{
          case SDR_skyParms               :			ShaderCommand_skyParms();              break;
          case SDR_cull                   :    		ShaderCommand_cull();				   break;
          case SDR_terrainfog             :			ShaderCommand_terrainfog();			   break;
          case SDR_deformVertexes         :			ShaderCommand_deformVertexes();		   break;
          case SDR_fogParams              :			ShaderCommand_fogParams();			   break;
          case SDR_terrainfogparams       :			ShaderCommand_terrainfogparams();	   break;
          case SDR_nopicmip               :			ShaderCommand_nopicmip();			   break;
          case SDR_nobright               :			ShaderCommand_nobright();			   break;
          case SDR_nomipmaps              :			ShaderCommand_nomipmaps();			   break;
          case SDR_polygonOffset          :			ShaderCommand_polygonOffset();		   break;
          case SDR_portal                 :			ShaderCommand_portal();				   break;
          case SDR_sort                   :			ShaderCommand_sort();				   break;
          case SDR_tessSize               :			ShaderCommand_tessSize();			   break;
          case SDR_q3map_backshader       :			ShaderCommand_q3map_backshader();	   break;
          case SDR_q3map_globaltexture    :			ShaderCommand_q3map_globaltexture();   break;
          case SDR_q3map_sun              :			ShaderCommand_q3map_sun();			   break;
          case SDR_q3map_surfaceLight     :			ShaderCommand_q3map_surfaceLight();	   break;
          case SDR_q3map_lightImage       :			ShaderCommand_q3map_lightImage();	   break;
          case SDR_q3map_lightSubdivide   :			ShaderCommand_q3map_lightSubdivide();  break;
          case SDR_surfaceParm            :			ShaderCommand_surfaceParm();		   break;
          case SDR_qer_editorimage        :			ShaderCommand_qer_editorimage();	   break;
          case SDR_qer_nocarve            :			ShaderCommand_qer_nocarve();		   break;
          case SDR_qer_trans              :			ShaderCommand_qer_trans();			   break;
          case SDR_backsided              :			ShaderCommand_backsided();			   break;
          case SDR_cloudParms             :			ShaderCommand_cloudParms();			   break;
          case SDR_entityMergable         :			ShaderCommand_entityMergable();		   break;
          case SDR_fogGen                 :			ShaderCommand_fogGen();				   break;
          case SDR_fogOnly                :			ShaderCommand_fogOnly();			   break;
          case SDR_light                  :			ShaderCommand_light();				   break;
          case SDR_lightning              :			ShaderCommand_lightning();			   break;
          case SDR_q3map_backsplash       :			ShaderCommand_q3map_backsplash();	   break;
          case SDR_q3map_flare            :			ShaderCommand_q3map_flare();		   break;
		/////////// rfd addins
		  case SDR_sky                    :			ShaderCommand_sky();				   break;
          case SDR_terrain                :			ShaderCommand_terrain();			   break;
          case SDR_stenciled_flat_mirror  :			ShaderCommand_stenciled_flat_mirror(); break;
          case SDR_surfaceColor           :			ShaderCommand_surfaceColor();		   break;
          case SDR_spriteParam            :			ShaderCommand_spriteParam();		   break;
          case SDR_include_effect         :			ShaderCommand_include_effect();		   break;
		/////////// Layer commands
		  case SDR_map                    :         LayerCommand_map();                    break;
		  case SDR_clampMap               :         LayerCommand_clampMap();			   break;
	 	  case SDR_animMap                :         LayerCommand_animMap();				   break;
	 	  case SDR_blendFunc              :         LayerCommand_blendFunc();			   break;
	 	  case SDR_rgbGen                 :         LayerCommand_rgbGen();				   break;
	 	  case SDR_alphaGen               :         LayerCommand_alphaGen();			   break;
	 	  case SDR_tcGen                  :         LayerCommand_tcGen();				   break;
	 	  case SDR_tcMod                  :         LayerCommand_tcMod();				   break;
		  case SDR_depthFunc              :         LayerCommand_depthFunc();			   break;
		  case SDR_depthMask              :         LayerCommand_depthMask();			   break;
		  case SDR_detail                 :         LayerCommand_detail();				   break;
		  case SDR_alphaFunc              :         LayerCommand_alphaFunc();			   break;
		////////////////////////
		  default:		InvalidToken();
		}
  	 //SkipLine();
	}
}


//
// Parse the layer
//
void CShaderParser::ParseLayer(void){
}
void CShaderParser::CloseLayer  (void){
}


//
// Parse a function
//
float CShaderParser::ParseFunc(void){

	if(!GetParameter()) return 0;

	     if(!stricmp(CParser::token, "sin"     ))		  return FUNC_SIN;
	else if(!stricmp(CParser::token, "triangle"))	   	  return FUNC_TRIANGLE;
	else if(!stricmp(CParser::token, "square"  ))		  return FUNC_SQUARE;
	else if(!stricmp(CParser::token, "sawtooth"))		  return FUNC_SAWTOOTH;
	else if(!stricmp(CParser::token, "inversesawtooth"))  return FUNC_INVERSESAWTOOTH;
	else if(!stricmp(CParser::token, "noise"   ))		  return FUNC_NOISE;
	else {
		InvalidToken();
		return 0;
	}
};


//
// Get shader name
//
bool CShaderParser::GetName(void){
	return CParser::GetToken(true);
}


//
// Get a command
//
bool CShaderParser::GetCommand(void){
	if(!CParser::GetToken(false)){
        ShaderError( "Ïğåäóïğåæäåíèå, command expected íà ëèíèè %d\n", CParser::scriptline);
		return false;
	}
	return true;
}


//
// Get a parameter
//
bool CShaderParser::GetParameter(void){
	if(!CParser::GetToken(false)){
        ShaderError( "Ïğåäóïğåæäåíèå, parameter expected íà ëèíèè %d\n", CParser::scriptline);
		return false;
	}
	return true;
}


//
// Error reporting, bat token
//
void CShaderParser::InvalidToken(void){
    ShaderError( "Ïğåäóïğåæäåíèå, invalid token: %s íà ëèíèè %d\n", CParser::token, CParser::scriptline);
}


//
// Skip line, all the parameters have been parsed, but there are more tokens on this line
//
void CShaderParser::SkipLine(void){
	while(CParser::GetToken(false)){
        ShaderError( "Ïğåäóïğåæäåíèå, skipping %s íà ëèíèè %d\n", CParser::token, CParser::scriptline);
	};
}


//
// Skip line, all the parameters have been parsed, but there are more tokens on this line
//
void CShaderParser::IgnoreLine(void){
	while(CParser::GetToken(false)){};
}







/*---------------------------------------------------------------------------
	Default implementation of all commands:
---------------------------------------------------------------------------*/

//
// general shader commands
//
void CShaderParser::ShaderCommand_skyParms(void){
/*
 Specifies how to use the surface as a sky, including an optional far box (stars, moon, etc),
 optional cloud layers with any shader attributes, and an optional near box (mountains in
 front of the clouds, etc).
 <Farbox> Specifies a set of files to use as an environment box behind all cloud layers.
  Specify "-" for no farbox, or a file base name.  A base name of "env/test" would look for
  files "env/test_rt.tga", "env/test_lf.tga", "env/test_ft.tga", "env/test_bk.tga",
  "env/test_up.tga", "env/test_dn.tga" to use as the right / left / front / back / up / down
  sides.
 <cloudheight> controls apparent curvature of the cloud layers - lower numbers mean more
  curvature (and thus more distortion at the horizons).  Higher height values create "flatter"
  skies with less horizon distortion.  Think of height as the radius of a sphere on which the
  clouds are mapped. Good ranges are 64 to 256.  The default value is 128.
 <nearbox>   Specified as farbox, to be alpha blended on top of the clouds.  This has not been
  tested in a long time, so it probably doesn't actually work.  Set to "-" to ignore.
*/
	ShaderNotImp();
}

void CShaderParser::ShaderCommand_cull(void){
/*
 Every surface of a polygon has two sides, a front and a back.  Typically, we only see the
 front or "out" side. For example, a solid block you only show the front side.  In many
 applications we see both. For example, in water, you can see both front and a back. The same
 is true for things like grates and screens.

 To "cull" means to remove. The value parameter determines the type of face culling to apply.
 The default value is cull front if this keyword is not specified.  However for items that
 should be inverted then the value back should be used.  To disable culling, the value disable
 or none should be used. Only one cull instruction can be set for the shader.
*/
	ShaderNotImp();
}

void CShaderParser::ShaderCommand_deformVertexes(void){
/*
 This function performs a general deformation on the surface's vertexes, changing the actual
 shape of the surface before drawing the shader passes.  You can stack multiple deformVertexes
 commands to modify positions in more complex ways, making an object move in two dimensions,
 for instance.


 ·deformVertexes wave <div> <func> <base> <amplitude> <phase> <freq>
  Designed for water surfaces, modifying the values differently at each point.  It accepts the
  standard wave functions of the type sin, triangle, square, sawtooth or inversesawtooth.
  The "div" parameter is used to control the wave "spread" - a value equal to the tessSize of
  the surface is a good default value (tessSize is subdivision size, in game units, used for
  the shader when seen in the game world).

     ÏĞÈÌÅĞ:

     deformVertexes wave 100 sin 0 3 0 .7 0 1 0

	 100 - ĞÀÇÌÅĞ ÂÎËÍÛ
	 sin - Ô-ÖÈß
	 0 -  ÑÌÅÙÅÍÈÅ ÖÅÍÒĞÀ ÌÀÑÑ
	 3 - ÈÑÊĞÈÂËÅÍÍÎÑÒÜ
	 0 - ?
	 7 - ÑÊÎĞÎÑÒÜ

	 ÄÀËÅÅ ÈÄÓÒ ÊÎÎĞÄÈÍÀÒÛ ÂÅÊÒÎĞÀ ÍÀÏĞÀÂËÅÍÈß ÄÅÔÎĞÌÀÖÈÈ
	 ÅÑËÈ ÎÍ ÎÏĞÅÄÅËÅÍ, ÒÎ ÂÅĞØÈÍÛ ÃÅÎÌÅÒĞÈÈ ÁÓÄÓÒ ÏĞÎÄÅÔÎĞÌÈĞÎÂÀÍÛ ÊÀÊ
	 ÅÄÈÍÎÅ ÒÅËÎ, ÈÍÀ×Å (ÅÑËÈ 0 0 0, ÒÎ ÂÅĞØÈÍÛ ÃÅÎÌÅÒĞÈÈ ÁÓÄÓÒ
	 ÏĞÎÄÅÔÎĞÌÈĞÎÂÀÍÛ ÈÍÄÈÂÈÄÓÀËÜÍÎ, ÍÅÑÎÃËÀÑÎÂÀÍÍÎ Ñ ÄĞÓÃÈÌÈ,
	 ÂÄÎËÜ ÂÅÊÒÎĞÀ ÑÂÎÅÉ ÍÎĞÌÀËÈ



 ·deformVertexes normal <div> <func> <base> <amplitude ~0.1-~0.5> <frequency ~1.0-~4.0>
  This deformation affects the normals of a vertex without actually moving it, which will
  effect later shader options like lighting and especially environment mapping.  If the shader
  stages don't use normals in any of their calculations, there will be no visible effect.


 ·deformVertexes bulge <bulgeWidth> <bulgeHeight> <bulgeSpeed>
  This forces a bulge to move along the given s and t directions.
  Designed for use on curved pipes.
  Specific parameter definitions for deform keywords:
   <div>  This is roughly defined as the size of the waves that occur. It is measured in game
    units. Smaller values create a greater density of smaller wave forms occurring in a given
    area.  Larger values create a lesser density of waves, or otherwise put, the appearance of
    larger waves.  To look correct this value should closely correspond to the value (in pixels)
    set for tessSize (tessellation size) of the texture.  A value of 100.0 is a good default
    value (which means your tessSize should be close to that for things to look "wavelike").
   <func> This is the type of wave form being created.  Sin stands for sine wave, a regular
    smoothly flowing wave.  Triangle is a wave with a sharp ascent and a sharp decay. It will
    make a choppy looking wave forms.  A square wave is simply on or off for the period of the
    frequency with no in between. The sawtooth wave has the ascent of a triangle wave, but has
    the decay cut off sharply like a square wave.  An inversesawtooth wave reverses this.
   <base>  This is the distance, in game units that the apparent surface of the texture is
    displaced from the actual surface of the brush as placed in the editor.
    A positive value appears above the brush surface. A negative value appears below the brush
    surface.
    An example of this is the Quad effect, which essentially is a shell with a positive base
    value to stand it away from the model surface and a 0 (zero) value for amplitude.
   <amplitude> The distance that the deformation moves away from the base value.  See Wave
    Forms in the introduction for a description of amplitude.
   <phase> See Wave Forms in the introduction for a description of phase)
   <frequency> See Wave Forms in the introduction for a description of frequency)


 ·deformVertexes move <x> <y> <z> <func> <base> <amplitude> <phase> <freq>
  This keyword is used to make a brush, curve patch or md3 model appear to move together as a
  unit.  The <x> <y> and <z> values are the distance and direction in game units the object
  appears to move relative to it's point of origin in the map.
  The <func> <base> <amplitude> <phase> and <freq> values are the same as found in other wave
  form manipulations.


 ·DeformVertexes sprite
  This function can be used to make any given triangle quad (pair of triangles that form a
  square rectangle) automatically behave like a sprite without having to make it a separate
  entity.  This means that the "sprite" on which the texture is placed will rotate to always
  appear at right angles to the player's view as a sprite would. Any four-sided brush side,
  flat patch, or pair of triangles in an .md3 model can have the sprite effect on it.
  The brush face containing a texture with this shader keyword must be square.


 ·DeformVertexes autosprite2
  Is a slightly modified "sprite" that only rotates around the middle of its longest axis.
  This allows you to make a pillar of fire that you can walk around, or an energy beam
  stretched across the room.


 ·DeformVertexes rotate xAngle yAngle zAngle
  *Angle óãîë ïîâîğîòà ïî ñîîòâ. îñè
  âîçìîæíû âàğèàíòû:
	1) DeformVertexes rotate 90 0 0
	ïîâîğîò îäèí ğàç

    2) DeformVertexes rotate timer*68 0 0
	ïîâîğîò â ğåàë-òàèìå ïî ñîîòâ. îñè ñî ñêîğîñòüş timer*68

    3) DeformVertexes rotate sin(timer*68)*12 0 0
	ïîâîğîò â ğåàë-òàèìå ïî ñîîòâ. îñè ïî ñèíóñó ñî ñêîğîñòüş
	timer*68 ñ ïåğèóäîì *12

  âîçìîæíî êàêîé íèáóäü ïàğàìåòğ èç 3 íå óêàçàí - îí áóäåò 0 ïî äåôîëòó!
*/
	ShaderNotImp();
}

void CShaderParser::ShaderCommand_fogParams(void){
/*
 Note: you must also specify "surfaceparm fog" to cause q3map to identify the surfaces inside
  the volume.  Fogparms only describes how to render the fog on the surfaces.

 <red value> <green value> <blue value> These are normalized values. A good computer art
  program should give you the RGB values for a color. To obtain the values that define fog
  color for Quake III Arena , divide the desired color's Red, Green and Blue values by 255 to
  obtain three normalized numbers within the 0.0 to 1.0 range.

 <distance to opaque> This is the distance, in  game units, until the fog becomes totally
  opaque, as measured from the point of view of the observer.  By making the height of the fog
  brush shorter than the distance to opaque ,  the apparent density of the fog can be reduced
 (because it never reaches the depth at which full opacity occurs)..
 ·The fog volume can only have one surface visible (from outside the fog).
 ·Fog must be made of one brush. It cannot be made of adjacent brushes.
 ·Fog brushes must be axial.  This means that only square or rectangular brushes may contain
  fog, and those must have their edges drawn along the axes of the map grid (all 90 degree
  angles).
*/
	ShaderNotImp();
}

void CShaderParser::ShaderCommand_nopicmip(void){
/*
 This causes the texture to ignore user-set values for the  r_picmip cvar command.  The image
 will always be high resolution.  Example:  Used to keep images and text in the heads up
 display from blurring when user optimizes the game graphics.
*/
	ShaderNotImp();
}

void CShaderParser::ShaderCommand_nomipmaps(void){
/*
 This implies nopicmip, but also prevents the generation of any lower resolution mipmaps for
 use by the 3d card.  This will cause the texture to alias when it gets smaller, but there are
 some cases where you would rather have this than a blurry image.  Sometimes thin slivers of
 triangles force things to very low mipmap levels, which leave a few constant pixels on
 otherwise scrolling special effects.
*/
	ShaderNotImp();
}

void CShaderParser::ShaderCommand_polygonOffset(void){
/*
 Surfaces rendered with the polygonOffset keyword are rendered slightly off the polygon's
 surface.  This is typically used for wall markings and "decals." The distance between the
 offset and the polygon is fixed.  It is not a variable in Quake III Arena.
*/
	ShaderNotImp();
}

void CShaderParser::ShaderCommand_portal(void){
/*
 Specifies that this texture is the surface for a portal or mirror.  In the game map, a portal
 entity must be placed directly in front of the texture (within 64 game units).  All this does
 is set "sort portal", so it isn't needed if you specify that explicitly.
*/
	ShaderNotImp();
}

void CShaderParser::ShaderCommand_sort(void){
/*
 Use this keyword to fine-tune the depth sorting of shaders as they are compared against other
 shaders in the game world. The basic concept is that if there is a question or a problem with
 shaders drawing in the wrong order against each other, this allows the designer to create a
 hierarchy of which shader draws in what order.

 The default behavior is to put all blended shaders in sort "additive" and all other shaders
 in sort "opaque", so you only need to specify this when you are trying to work around a
 sorting problem with multiple transparent surfaces in a scene.

 The value here can be either a numerical value or one of the keywords in the following list
 (listed in order of ascending priority):

  portal (1): This surface is a portal, it draws over every other shader seen inside the
   portal, but before anything in the main view.
  Sky (2): Typically, the sky is the farthest surface in the game world.  Drawing this after
   other opaque surfaces can be an optimization on some cards.  This currently has the wrong value for this purpose, so it doesn't do much of anything.
  Opaque (3): This surface is opaque (rarely needed since this is the default with no
   blendfunc)
  Banner (6) : Transparent, but very close to walls.
  Underwater (8): Draw behind normal transparent surfaces.
  Additive (9):  normal transparent surface (default for shaders with blendfuncs)
  nearest (16): this shader should always sort closest to the viewer, e.g. muzzle flashes and
   blend blobs
*/
	ShaderNotImp();
}


// map processing tools shader commands
void CShaderParser::ShaderCommand_tessSize(void){
/*
 For consistency's sake, this really should have been called q3map_tessSize. But it wasn't.
 The tessSize shader controls the tessellation size (how finely a surface is chopped up in to
 triangles), in game units, of the surface.  This is only applicable to solid brushes, not
 curves, and is generally only used on surfaces that are flagged with the deformVertexes
 keyword.  Abuse of this can create a huge number of triangles.  This happens during q3map
 processing, so maps must be reprocessed for changes to take effect.
*/
	ShaderNotImp();
}

void CShaderParser::ShaderCommand_q3map_backshader(void){
/*
 This allows a brush to use a different shader when you are inside it looking out. By way of
 example, this would allow a water brush (or other) surfaces to have a different sort order
 (see sort above) or appearance when seen from the inside.
*/
	ShaderNotImp();
}

void CShaderParser::ShaderCommand_q3map_globaltexture(void){
/*
 Use this shader in the global keyword commands whenever the tcMod scale function is used in
 one of the later render stages. Many problems with getting shader effects to work across
 multiple adjacent brushes are a result of the way q3map optimizes texture precision. This
 option resolves that, but at the expense of some precision of the textures when they are far
 away from the origin of the map.
*/
	ShaderNotImp();
}

void CShaderParser::ShaderCommand_q3map_sun(void){
/*
 This keyword in a sky shader will create the illusion of light cast into a map by a single,
 infinitely distance light source (sun, moon, hellish fire, etc.).  This is only processed
 during the lighting phase of q3map.

 <red> <green> <blue> Color is described by three normalized rgb values.
  Color will be normalized to a 0.0 to 1.0 range, so it doesn't matter what range you use.
 <intensity> is the brightness of the generated light.
  A value of 100 is a fairly bright sun.
  The intensity of the light falls off with angle but not distance.
 <degrees> is the angle relative to the directions on the map file.
  A setting of 0 degrees equals east. 90 is north, 180 is west and 270 is south.
 <elevation> is the distance, measured in degrees from the horizon (z value of zero in the map
  file).  An elevation of 0 is sunrise/sunset. An elevation of 90 is noon
*/
	ShaderNotImp();
}

void CShaderParser::ShaderCommand_q3map_surfaceLight(void){
/*
 The texture gives off light equal to the <light value> set for it.
 The relative surface area of the texture in the world affects the actual amount of light
 that appears to be radiated.  To give off what appears to be the same amount of light, a
 smaller texture must be significantly brighter than a larger texture. Unless the
 qer_lightimage keyword is used to select a different source for the texture's light color
 information, the color of the light will be the averaged color of the texture
*/
	ShaderNotImp();
}

void CShaderParser::ShaderCommand_q3map_lightImage(void){
/*
 The keyword q3map_lightimage generates lighting from the average color of the TGA image
 specified by the  q3map_lightimage.
 The keyword sequence for generating light on a q3map_surfacelight should be ordered as
 follows:
 1) q3map_lightimage ; (the texture providing the light and the color of the light)
 2) qer_editorimage ; (the editor-only image used to select the source map for the texture)
 3) the average color of the light emitted from the shader is calculated from the lightimage.)
 The reason q3map_lightimage is specified for the light in the example below, is because the
 blend map is predominantly yellow, and the designer wanted more yellow light to be emitted
 from the light
*/
	ShaderNotImp();
}

void CShaderParser::ShaderCommand_q3map_lightSubdivide(void){
/*
 This allows the user to define how large, or small to make the subdivisions (triangles) in a
 textured surface, particularly aimed at light-emitting textures like skies.
 It defaults to 120 game units, but can be made larger (256 or 512) for sky boxes or smaller
 for light surfaces at the bottoms of cracks.  This can be a dominant factor in processing
 time for q3map lighting
*/
	ShaderNotImp();
}

void CShaderParser::ShaderCommand_surfaceParm(void){
/*
 All surfaceparm keywords are preceded by the word surfaceparm as follows: surfaceparm fog or
 surfaceparm noimpact.
 ·alphashadow
  This keyword applied to a texture on a brush, patch or model will cause the lighting phase
  of the q3map process to use the texture's alpha channel as a mask for casting static shadows
  in the game world.
 ·areaportal
  A brush marked with this keyword functions as an area portal, a break in the Q3MAP tree.
  It is typically placed on a very thin brush placed inside a door entity (but is not a part
  of that entity).
  The intent is to block the game from processing surface triangles located behind it when
  the door is closed.  It is also used by the BSPC (bot area file creation compiler) in the
  same manner as a cluster portal. The brush must touch all the structural brushes surrounding
  the areaportal.
 ·clusterportal
  A brush marked with this keyword function creates a subdivision of the area file (.aas) used
  by the bots for navigation. It is typically placed in locations that are natural breaks in a
  map, such as entrances to halls, doors, tunnels, etc. The intent is keep the bot from having
  to process the entire map at once. As with the the areaportal parameter, the affected brush
  must touch all the structural brushes surrounding the areaportal.
 ·donotenter
  Read as "do not enter." Like clusterportal, this is a bot-only property. A brush marked with
  donotenter will not affect non-bot players, but bots will not enter it.
  It should be used only when bots appear to have difficulty navigating around some map
  features.
 ·flesh
  This will cue different sounds (in a similar manner to metalsteps ) and cause blood to appear
  instead of bullet impact flashes.
 ·fog
  Fog defines the brush as being a "fog" brush.
  This is a Q3MAP function that chops and identifies all geometry inside the brush. The
  General shader keyword fogparms must also be specified to tell how to draw the fog.
 ·lava
  Assigns to the texture the game properties set for lava.
  This affects both the surface and the content of a brush.
 ·metalsteps
  The player sounds as if he is walking on clanging metal steps or gratings. Other than
  specifiying flesh, metalsteps, nosteps, or default (i.e.; specify nothing) it is currently
  not possible for a designer to create or assign a specific sound routine to a texture.
  Note: If no sound is set for a texture, then the default footsteps sound routines are heard.
 ·nodamage
  The player takes no damage if he falls onto a texture with this surfaceparm
 ·nodlight
  Read as "No Dee Light". A texture containing this parameter will not be affected or lit by
  dynamic lights, such as weapon effects.
  And example in Quake III Arena  would be solid lava.
 ·nodraw
  A texture marked with nodraw will not visually appear in the game world.
  Most often used for triggers, clip brushes, origin brushes, and so on.
 ·nodrop
  When a player dies inside a volume (brush) marked nodrop, no weapon is dropped.
  The intend use is for "Pits of Death."  Have a kill trigger inside a nodrop volume, and when
  the players die here, they won't drop their weapons. The intent is to prevent unnecessary
  polygon pileups on the floors of pits.
 ·noimpact
  World entities will not impact on this texture.
  No explosions occur when projectiles strike this surface and no marks will be left on it.
  Sky textures are usually marked with this texture so those projectiles will not hit the sky
  and leave marks.
 ·nomarks
  Projectiles will explode upon contact with this surface, but will not leave marks.
  Blood will also not mark this surface.  This is useful to keep lights from being temporarily
  obscured by battle damage.
 ·nolightmap
  This texture does not have a lightmap phase.
  It is not affected by the ambient lighting of the world around it. It does not require the
  addition of an rgbGen identity keyword in that stage.
 ·nosteps
  The player makes no sound when walking on this texture.
 ·nonsolid
  This attribute indicates a brush, which does not block the movement of entities in the game
  world.
  It applied to triggers, hint brushes and similar brushes.
  This affects the content of a brush.
 ·origin
  Used on the "origin" texture.  Rotating entities need to contain an origin brush in their
  construction. The brush must be rectangular (or square).
  The origin point is the exact center of the origin brush.
 ·playerclip
  Blocks player movement through a nonsolid texture.
  Other game world entities can pass through a brush marked playerclip. The intended use for
  this is to block the player but not block projectiles like rockets.
 ·slick
  This surfaceparm included in a texture should give it significantly reduced friction.
 ·slime
  Assigns to the texture the game properties for slime. This affects both the surface and the
  content of a brush.
 ·structural
  This surface attribute causes a brush to be seen by the Q3MAP process as a possible
  break-point in a BSP tree.
  It is used as a part of the shader for the "hint" texture.
  Generally speaking, any opaque texture not marked as "detail" is by default, structural, so
  you shouldn't need to specify this.
 ·trans
  Tells q3map that pre-computed visibility should not be blocked by this surface.
  Generally, any shaders that have blendfuncs should be marked as surfaceparm trans.
 ·water
  Assigns to the texture the game properties for water.
*/
	ShaderNotImp();
}


//
// qeradiant shadercommands
//
void CShaderParser::ShaderCommand_qer_editorimage(void){
/*
 This keyword creates a shader name in memory, but in the editor, it displays the TGA art
 image specified in qer_editorimage (in the example below this is textures/eerie/lavahell.tga)
*/
	ShaderNotImp();
}

void CShaderParser::ShaderCommand_qer_nocarve(void){
/*
 A brush marked with this instruction will not be affected by CSG subtract functions.
 It is especially useful for water and fog textures
*/
	ShaderNotImp();
}

void CShaderParser::ShaderCommand_qer_trans(void){
/*
 This parameter defines the percentage of transparency that a brush will have when seen in the
 editor (no effect on game rendering at all).
 It can have a positive value between 0 and 1.
 The higher the value, the less transparent the texture
*/
	ShaderNotImp();
}


//
// undocumented shader commands
//
void CShaderParser::ShaderCommand_backsided(void){
    ParseError( "Ïğåäóïğåæäåíèå: Undocumented command %s appears íà ëèíèè %d\n", CParser::token, CParser::scriptline);
	ShaderNotImp();
}

void CShaderParser::ShaderCommand_cloudParms(void){
    ParseError( "Ïğåäóïğåæäåíèå: Undocumented command %s appears íà ëèíèè %d\n", CParser::token, CParser::scriptline);
	ShaderNotImp();
}

void CShaderParser::ShaderCommand_entityMergable(void){
    ParseError( "Ïğåäóïğåæäåíèå: Undocumented command %s appears íà ëèíèè %d\n", CParser::token, CParser::scriptline);
	ShaderNotImp();
}

void CShaderParser::ShaderCommand_fogGen(void){
    ParseError( "Ïğåäóïğåæäåíèå: Undocumented command %s appears íà ëèíèè %d\n", CParser::token, CParser::scriptline);
	ShaderNotImp();
}

void CShaderParser::ShaderCommand_fogOnly(void){
    ParseError( "Ïğåäóïğåæäåíèå: Undocumented command %s appears íà ëèíèè %d\n", CParser::token, CParser::scriptline);
	ShaderNotImp();
}

void CShaderParser::ShaderCommand_light(void){
    ParseError( "Ïğåäóïğåæäåíèå: Undocumented command %s appears íà ëèíèè %d\n", CParser::token, CParser::scriptline);
	ShaderNotImp();
}

void CShaderParser::ShaderCommand_lightning(void){
    ParseError( "Ïğåäóïğåæäåíèå: Undocumented command %s appears íà ëèíèè %d\n", CParser::token, CParser::scriptline);
	ShaderNotImp();
}

void CShaderParser::ShaderCommand_q3map_backsplash(void){
    ParseError( "Ïğåäóïğåæäåíèå: Undocumented command %s appears íà ëèíèè %d\n", CParser::token, CParser::scriptline);
	ShaderNotImp();
}

void CShaderParser::ShaderCommand_q3map_flare(void){
    ParseError( "Ïğåäóïğåæäåíèå: Undocumented command %s appears íà ëèíèè %d\n", CParser::token, CParser::scriptline);
	ShaderNotImp();
}

//
// layer commands
//
void CShaderParser::LayerCommand_map(void){
/*
 Specifies the source texture map (a 24 or 32-bit TGA file) used for this stage.
 The texture may or may not contain alpha channel information.
 The special keywords $lightmap and $whiteimage may be substituted in lieu of an actual texture
 map name. In those cases, the texture named in the first line of the shader becomes the
 texture that supplies the light mapping data for the process.
*/
	if(!CParser::GetToken(false)){ParseError( "Warning: map not set");return;}
	//Shader->AddMap(CParser::token);
}

void CShaderParser::LayerCommand_clampMap(void){
/*
*/
	ShaderNotImp();
	if(!CParser::GetToken(false)){ParseError( "Warning: clampMap not set");return;}
}

void CShaderParser::LayerCommand_animMap(void){
/*
 The surfaces in the game can be animated by displaying a sequence of 1 to 8 frames (separate
 texture maps).
 These animations are affected by other keyword effects in the same and later shader stages.

 <Frequency>: the number of times that the animation cycle will repeat within a one second
  time period.
  The larger the value, the more repeats within a second. Animations that should last for more
  than a second need to be expressed as decimal values.

 <texture1>..<texture8>:  the texturepath/texture name for each animation frame must be
  explicitly listed. Up to eight frames (eight separate .tga files) can be used to make an
  animated sequence. Each frame is displayed for an equal subdivision of the frequency value.
*/
	ShaderNotImp();
	if(!CParser::GetToken(false)){ParseError( "Warning: AminMap not set");return;}
}

void CShaderParser::LayerCommand_blendFunc(void){
/*
 Blend functions are the keyword commands that tell the Quake III Arena graphic engine's
 renderer how graphic layers are to be mixed together

 ·blendfunc add
  This is a shorthand command for blendfunc gl_one gl_one.
  Effects like fire and energy are additive

 ·blendfunc filter
  This is a shorthand command that can be substituted for either  blendfunc gl_dst_color
  gl_zero or blendfunc gl_zero gl_src_color.
  A filter will always result in darker pixels than what is behind it, but it can also remove
  color selectively.
  Lightmaps are filters

 ·blendfunc blend
  Shorthand for blendfunc gl_src_alpha gl_one_minus_src_alpha.
  This is conventional transparency, where part of the background is mixed with part of the
  texture.

  .blendfunc modulate2x
  Äëÿ äâîéíîãî óìíîæåíèÿ èñïîëüçîâàëàñü ôóíêöèÿ òåêñòóğíîãî áëåíäà
   Shorthand for GL_DST_COLOR GL_SRC_COLOR .

 ·Explicit blend functions: [Source * <srcBlend>] + [Destination * <dstBlend>]
  Source Blend <srcBlend>
   GL_ONE
   GL_ZERO
   GL_DST_COLOR
   GL_ONE_MINUS_DST_COLOR
   GL_SRC_ALPHA
   GL_ONE_MINUS_SRC_ALPHA
  Destination Blend <dstBlend>
   GL_ONE
   GL_ZERO
   GL_SRC_COLOR
   GL_ONE_MINUS_SRC_COLOR
   GL_SRC_ALPHA
   GL_ONE_MINUS_SRC_ALPHA
  The Riva 128 graphics card supports ONLY the following blend modes:
   GL_ONE, GL_ONE
   GL_DST_COLOR, GL_ZERO
   GL_ZERO, GL_SRC_COLOR
   GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA
   GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA
  Cards running in 16 bit color cannot use any GL_DST_ALPHA blends.
*/
	ShaderNotImp();
}

void CShaderParser::LayerCommand_rgbGen(void){
/*
 .identitylighting
  Colors will be (0.5, 0.5, 0.5) if running without overbright bits (NT, linux, windowed modes),
  or (1, 1, 1) if running with overbrig
  .identitylightinginverse
  mozet prigoditsja dlja normalmappinga
  Colors will be (1.0,1.0,1.0) if running without overbright bits (NT, linux, windowed modes),
  or (0.5, 0.5, 0.5) if running with overbrig
 ·identity
  Colors are assumed to be all white (1.0,1.0,1.0).
  All filters stages (lightmaps, etc) will get this by default
 ·wave <func> <base> <amp> <phase> <freq>
  Colors are generated using the specified waveform
 ·entity
  Colors are grabbed from the entity's modulate field
 ·oneMinusEntity
  Colors are grabbed from 1.0 minus the entity's modulate field
 ·Vertex
  Colors are filled in directly by the data from the map or model files
 ·oneMinusVertex
  As rgbGen vertex, but inverted
 ·lightingDiffuse
  Colors are computed using a standard diffuse lighting equation. It uses the vertex normals
  to illuminate the object correctly.
*/
	ShaderNotImp();
}

void CShaderParser::LayerCommand_alphaGen(void){
/*
 defaults to 1.0.
 ·portal
  The function accomplishes the "fade" that causes the scene in the portal to fade from view.
  Specifically, it means "Generate alpha values based on the distance from the viewer to the
  portal." Use alphaGen portal on the last rendering pass.
*/
	ShaderNotImp();
}

void CShaderParser::LayerCommand_tcGen(void){
/*
 <base> = base texture coordinates from the original art.
 <lightmap> = lightmap texture coordinates
 <environment> = Make this object environment mapped.
 vector (<sx> <sy> <sz>) (<tx> <ty> <tz>)
  New texcoord generation by world projection. This allows you to project a
  texture onto a surface in a fixed way, regardless of its orientation.
  S coordinates correspond to the "x" coordinates on the texture itself.
  T coordinates correspond to the "y" coordinates on the texture itself.
  The measurements are in game units.
*/
	ShaderNotImp();
}

void CShaderParser::LayerCommand_tcMod(void){
/*
 rotate <degrees per per second>
 scale <sScale> <tScale>
 scroll <sSpeed> <tSpeed>
 stretch <func> <base> <amplitude> <phase> <frequency>
 transform <m00> <m01> <m10> <m11> <t0> <t1>
  S' = s * m00 + t * m10 + t0
  T' = t * m01 + s * m11 + t1
 turb <base> <amplitude> <phase> <freq>
*/
	ShaderNotImp();
}

void CShaderParser::LayerCommand_depthFunc(void){
/*
 depthFunc <func>
 default: "lequal"
 "equal"
*/
	ShaderNotImp();
}

void CShaderParser::LayerCommand_depthMask(void){
/*
 Writes to the depth buffer when depthFunc passes will happen for opaque surfaces and not for
 translucent surfaces.
 Blended surfaces can have the depth writes forced with this function
*/
	ShaderNotImp();
}

void CShaderParser::LayerCommand_detail(void){
/*
 do not draw this layer, if r_detail is off
*/
	ShaderNotImp();
}

void CShaderParser::LayerCommand_alphaFunc(void){
/*
 alphaFunc <func>
 GT0, LW0.5, and GE0.5
*/
	ShaderNotImp();
}


//
// undocumented layer commands
//
void CShaderParser::LayerCommand_alphaMap(void){
    ParseError( "Ïğåäóïğåæäåíèå: Undocumented command %s appears íà ëèíèè %d\n", CParser::token, CParser::scriptline);
	ShaderNotImp();
}

///////////////// RFD ADDINS ////////////////////////
////////////////////// TERRAIN SHADER //////////////////
// ğàáîòàåò òîëüêî ñ "surfaceparm terrain"

void CShaderParser::ShaderCommand_terrainfog(void)
{
	/*
	terrainfog arg1

	arg1: false - true (çàïğåùàåì ğàçğåøàåì òóìàí íà óğîâíå
	(ñî âêëş÷åííûì ïàğàìåòğîì äîñòèãàåòñÿ ïîâûøåíèå
	ïğîèçâîäèòåëüíîñòè òàê êàê âñå ÷òî â òóìàíå ïğîñòî íàõåğ
	îòñåêàåòñÿ! - äğóãîé àëãîğèòì ğåíäåğà)
	*/

	ShaderNotImp();
}

void CShaderParser::ShaderCommand_terrainfogparams(void)
{
	/*
	ÏËÎÑÊÈÉ ÒÓÌÀÍ
	terrainfogparams arg1 arg2 arg3 arg4 arg5 arg6 arg7

	arg1:  red color
	arg2:  green color
	arg3:  blue color
	arg4:  alpha color
	arg5:  density
	arg6:  GL_FOG_START
	arg7:  GL_FOG_END
	*/

	ShaderNotImp();
}

void CShaderParser::ShaderCommand_terrain(void){
	/*
	Ó ÒÀÊÎÃÎ ØÀÄÅĞÀ ÄÎËÆÍÛ ÁÛÒÜ ÑÒĞÎÊÈ
	SURFACEPARM TERAIN
	terrainfog arg1
	terrainfogparams arg1 arg2 arg3 arg4 arg5 arg6 arg7
	À ÔÀÉË ÊÀĞÒÛ ÂÛÒÎÒ ÄÎËÆÅÍ ÁÛÒÜ Â BMP Ñ ÌÀÊÑ. ĞÀÇÌÅĞÀÌÈ 105*105
	*/
	ShaderNotImp();
}

void CShaderParser::ShaderCommand_sky(void)
{
	/*
	ÏĞÎÑÒÎ ÍÅÁÎ
	*/
	ShaderNotImp();
}

////////////////////// flat mirror 4erez stencil bufer shader //////////////////
void CShaderParser::ShaderCommand_stenciled_flat_mirror(void)
{
	/*
	ÏËÎÑÊÎÅ ÇÅĞÊÀËÎ
	Ó ÒÀÊÎÃÎ ØÀÄÅĞÀ ÄÎËÆÍÛ ÁÛÒÜ ÑÒĞÎÊÈ:
	surfaceparm flat_mirror
	flat_mirrorparams arg1 arg2 arg3 arg4
	blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA (â layer'e ñ òåêñòóğîé)
	*/
	ShaderNotImp();
}

void CShaderParser::ShaderCommand_surfaceColor(void)
{
	/*
	ÓÑÒÀÍÀÂËÈÂÀÅÒ ÖÂÅÒ ÏÎÂÅĞÕÍÎÑÒÈ [0-255] (byte)
	surfaceColor arg1 arg2 arg3 arg4

	Áóäåò ğàáîòàòü òîëüêî â ïàğå ñ rgbgen vertex

	// ÖÂÅÒ ÏÎÂÅĞÕÍÎÑÒÈ
	arg1:  red color
	arg2:  green color
	arg3:  blue color

	arg4:  alpha color <= ÷åì áîëüøå ıòîò ïîêàçàòåëü íåïğîçğà÷íîñòè,
	òåì õóæå áóäåò îòğàæàòü ïîâåğõíîñòü.

	Ê ÑÒÅÍÑÈËÜÍÛÌ ÇÅĞÊÀËÀÌ - íàïğèìåğ äëÿ ëóæè ëó÷øå âçÿòü 255*0.3f,
	à äëÿ ñòîëà 255*0.7f.
	*/

	ShaderNotImp();
}

void CShaderParser::ShaderCommand_spriteParam()
{
	/*
	ÏÀĞÀÌÅÒĞÛ(ÂÈÄÛ) ÑÏĞÀÈÒÎÂ

	UDLR_ROTATION_TO_VIEWER - ÏÎÂÎĞÎÒ ËÈÖÎÌ Ê ÍÀÁËŞÄÀÒÅËŞ ÏÎ
	ÍÀÏĞÀÂËÅÍÈßÌ: ÂÅĞÕ, ÍÈÇ , ËÅÂÎ, ÏĞÀÂÎ (ÍÀÏĞÈÌÅĞ FLARE'S)
	*/
	ShaderNotImp();
}

void CShaderParser::ShaderCommand_nobright(void){
	/*
	esli prisutstvuet to vse *map's v etom shadere ne budut
	overbraignuti r_overBrightBits
	*/
	ShaderNotImp();
}

void CShaderParser::ShaderCommand_include_effect()
{
	/*
	ÍÅÊÎÒÎĞÛÅ ÌÀÒÅĞÈÀËÛ ÌÎÃÓÒ ÂÇÀÈÌÎÄÅÉÑÒÂÎÂÀÒÜ ÑÎ ÑÏÅÖÅÔÔÅÊÒÀÌÈ
	İÒÈ ÊÎÌÀÍÄÛ ĞÀÁÎÒÀŞÒ ÒÎËÜÊÎ ÑÎ ÑÏĞÀÉÒÀÌÈ

	sunflare						SUN FLARE EFFECT (îñëåïëÿåò èãğîêà êîãäà ñìîòğèò íà ëàéò)
	sprite_size_linear_to_viewer	ĞÀÇÌÅĞ ÊÂÀÄÀ ÓÌÅÍÜØÀÅÒÑß Ñ ÏĞÈÁËÈÆÅÍÈÅÌ ÍÀÁËŞÄÀÒÅËß
	lensflare						LENS FLARE EFFECT (íå îñëåïëÿåò èãğîêà êîãäà ñìîòğèò íà ëàéò)
	*/
}

char SPszError[1024*10];

void    CShaderParser::ShaderError(LPCSTR Error,...)
{
    /*va_list ap;
    va_start(ap, Error);
    vsprintf(SPszError, Error, ap);
    va_end(ap);
	*/
	DOFORMAT(Err,Error);
	LPCSTR SPszError=Err;
	Error2Console(SYS_SHADERS,"SE%d|%d:%s",CParser::scriptline,Depth,SPszError);
}
void    CShaderParser::ParseError (LPCSTR Error,...)
{	
    /*va_list ap;
    va_start(ap, Error);
    vsprintf(SPszError, Error, ap);
    va_end(ap);
	*/
	DOFORMAT(Err,Error);
	LPCSTR SPszError=Err;
	Error2Console(SYS_SHADERS,"PE%d|%d:%s",CParser::scriptline,Depth,SPszError);
}
void    CShaderParser::ShaderNotImp()
{
	Error2Console(SYS_SHADERS,"%d|%d:This (%s) shader not implimented",CParser::scriptline,Depth,CParser::token);
	//SkipLine();
}
