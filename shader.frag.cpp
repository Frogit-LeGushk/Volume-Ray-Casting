#version 110

varying vec4 position;
uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform vec3 u_position;
uniform float u_time;

const float MAX_DIST = 999999999.0;

mat2 rot(float a)
{
    float s = sin(a);
    float c = cos(a);
    return mat2(c, -s, s, c);
}
vec2 sphIntersect(in vec3 ro, in vec3 rd, float ra)
{
    float b = dot(ro, rd);
    float c = dot(ro, ro) - ra * ra;
    float h = b * b - c;
    // the beam did not hit the sphere
    if(h < 0.0) return vec2(-1.0);
    // return distance between camera and sphere
    h = sqrt(h);
    return vec2(-b-h, -b+h);
}
vec2 boxIntersection(in vec3 ro, in vec3 rd, in vec3 rad, out vec3 oN)  {
	vec3 m = 1.0 / rd;
	vec3 n = m * ro;
	vec3 k = abs(m) * rad;
	vec3 t1 = -n - k;
	vec3 t2 = -n + k;
	float tN = max(max(t1.x, t1.y), t1.z);
	float tF = min(min(t2.x, t2.y), t2.z);
	if(tN > tF || tF < 0.0) return vec2(-1.0);
	oN = -sign(rd) * step(t1.yzx, t1.xyz) * step(t1.zxy, t1.xyz);
	return vec2(tN, tF);
}
float plaIntersect(in vec3 ro, in vec3 rd, in vec4 p) {
	return -(dot(ro, p.xyz) + p.w) / dot(rd, p.xyz);
}
vec3 getSky(vec3 rd, vec3 light)
{
    vec3 col = vec3(0.3, 0.6, 1.0);
    vec3 sun = vec3(0.95, 0.9, 1.0);
    sun *= pow(max(0.0, dot(rd, light)), 32.0);
    return clamp(col+sun, 0.0, 1.0);
}

vec3 castRay(inout vec3 ro, inout vec3 rd)
{
    vec3 col;
    vec2 minIt = vec2(MAX_DIST);
    vec2 it;
    vec3 n;
    vec3 light;
    vec3 reflected;

    /** 1) Calculate intersection with sphere (1 object) **/
    vec3 sphPos = vec3(0.0, 0.0, 1.5);
    it = sphIntersect(ro-sphPos, rd, 1.0);
    // Detected intersection with sphere (it.x >= 0.0)
    if(it.x > 0.0 && it.x < minIt.x)
    {
        minIt = it;
        vec3 itPos = ro + rd * it.x;
        // sphere have R=1 and {x=0-itPos.x,y=0-itPos.y,z=0-itPos.z}
        n = itPos-sphPos;
        col = vec3(0.9, 0.6, 0.3);
    }

    /** 2) Calculate intersection with cube (2 object) **/
    vec3 boxN;
    vec3 boxPos = vec3(0.0, 0.0, -1.5);
    it = boxIntersection(ro-boxPos, rd, vec3(1.0), boxN);
    // Detected intersection with box (it.x >= 0.0)
    if(it.x > 0.0 && it.x < minIt.x)
    {
        minIt = it;
        n = boxN;
        col = vec3(0.3, 0.6, 0.9);
    }

    /** 3) Calculate intersection with plane as floor (3 object) **/
    vec3 planeN = vec3(0.0, 1.0, 0.0);
    it = vec2(plaIntersect(ro, rd, vec4(planeN, 1.0)));
    // Detected intersection with plane (it.x >= 0.0)
    if(it.x > 0.0 && it.x < minIt.x)
    {
        minIt = it;
        n = planeN;
        col = vec3(0.5);
    }

    // light 1
    light = normalize(vec3(cos(u_time), 0.75, sin(u_time)));
    float diffuse = max(0.0, dot(light, n)) * 0.5 + 0.035;

    // the ray did not intersection with sphere or box
    if(minIt.x == MAX_DIST)
        return vec3(-1.0);

    // light 2
    float specular = pow(max(0.0, dot(reflect(rd, n), light)), 32.0);

    col *= mix(vec3(diffuse), vec3(specular), 0.5);
    ro += rd * (minIt.x-0.001);
    return col;
}

vec3 traceRay(vec3 ro, vec3 rd)
{
    vec3 light = normalize(vec3(cos(u_time), 0.75, sin(u_time)));
    vec3 col = castRay(ro, rd);
    if(col.x == -1.0) return getSky(rd, light);
    vec3 lightDir = light;
    if(castRay(ro, lightDir).x != -1.0) col *= 0.5;

    return col;
}

void main(void)
{
    vec2 uv = position.yx;
    uv.y *= u_resolution.x/u_resolution.y;

    vec3 rayOrigin = u_position;
    vec3 rayDirection = normalize(vec3(1.0, uv));

    rayDirection.xy *= rot(u_mouse.y);
    rayDirection.zx *= rot(-u_mouse.x);

    //vec3 col = castRay(rayOrigin, rayDirection);
    vec3 col = traceRay(rayOrigin, rayDirection);

    col.r = pow(col.r, 0.45);
    col.g = pow(col.g, 0.45);
    col.b = pow(col.b, 0.45);

    gl_FragColor = vec4(col, 1.0);
}
