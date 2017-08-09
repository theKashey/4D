#ifndef _FLARING_H_
#define _FLARING_H_

/*void AddNeonMeshFlare(const float* fconstant_light_color,
					  const float* position,
					  float flare_w,
					  float flare_h,
					  float flare_r);
*/
void AddCoronaFlare  (const float* color,
					  const float* position,
					  float start_fallof_factor,
					  float max_radius,
					  DWORD tex_id);

#endif