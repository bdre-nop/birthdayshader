#version 330 core
out vec4 O;
in vec2 fragCoord;
uniform vec2 iResolution;
uniform float iTime;
uniform float uScale;
uniform float uRandom;

#define PI 3.1415926535
//NOTE The SAT() macro from the original shadertoy this was based off (see below) had the
//  parameters for clamp REVERSED (0.0, 1.0, x) which is.. non-sensical but somehow still
//  worked on Apple and Intel silicon but not with nVidia
#define C(x) clamp(x, 0.0, 1.0)
#define S(a, b, x) smoothstep(a, b, x)

#define SETUP_LETTER(LX, LY, LA, LS, LXINC, LYINC) { \
    st = vec2(LX, LY); \
    angle = LA; \
    spiral = LS; \
    st.x += LXINC; \
    st.y += LYINC; \
    topGrad = 1.3 - S(0.0, 1.0, C(pow(3.0 * st.y - uv.y,2))); \
    botGrad = 0.7 + S(0.0, 1.0, C(pow(3.0 * st.y - uv.y,2))); }


mat2 rot(float a) {
    return mat2(cos(a), -sin(a), sin(a), cos(a));
}


/***************************** Background functions *****************************/

// Based on code from https://www.shadertoy.com/view/wdyczG

vec2 hash(vec2 p) {
    p = vec2( dot(p,vec2(2127.1,81.17)), dot(p,vec2(1269.5,283.37)) );
	return fract(sin(p)*43758.5453);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
	vec2 u = f*f*(3.0-2.0*f);
    float n = mix( mix( dot( -1.0+2.0*hash( i + vec2(0.0,0.0) ), f - vec2(0.0,0.0) ), 
                        dot( -1.0+2.0*hash( i + vec2(1.0,0.0) ), f - vec2(1.0,0.0) ), u.x),
                   mix( dot( -1.0+2.0*hash( i + vec2(0.0,1.0) ), f - vec2(0.0,1.0) ), 
                        dot( -1.0+2.0*hash( i + vec2(1.0,1.0) ), f - vec2(1.0,1.0) ), u.x), u.y);
	return 0.5 + 0.5*n;
}

vec3 drawBackground() {
    vec2 uv = (fragCoord/iResolution.xy) - 0.5;
    float ratio = iResolution.x / iResolution.y;

    // rotate with Noise (and use provided random seed and a 'special number')
    float degree = noise(vec2((iTime + (uRandom * 2002.411)) * 0.08, uv.x*uv.y));

    uv.y *= 1./ratio;
    uv *= rot(radians((degree-.5)*720.+180.));
	uv.y *= ratio;

    // Wave warp with sin
    float frequency = 5.;
    float amplitude = 30.;
    float speed = iTime * 2.;
    uv.x += sin(uv.y*frequency+speed)/amplitude;
   	uv.y += sin(uv.x*frequency*1.5+speed)/(amplitude*.5);
    
    // draw the image
    vec3 colorYellow = vec3(.957, .804, .623);
    vec3 colorDeepBlue = vec3(.192, .384, .933);
    vec3 layer1 = mix(colorYellow, colorDeepBlue, S(-.3, .2, (uv*rot(radians(-5.))).x));
    
    vec3 colorRed = vec3(.910, .510, .8);
    vec3 colorBlue = vec3(0.350, .71, .953);
    vec3 layer2 = mix(colorRed, colorBlue, S(-.3, .2, (uv*rot(radians(-5.))).x));
    
    return mix(layer1, layer2, S(.5, -.3, uv.y));
} // drawBackground


/***************************** 'Letterbox' ;-) functions *****************************/

// Based on code from https://www.shadertoy.com/view/3sByD1

float sdBox(vec2 p, vec2 b)
{
    vec2 d = abs(p) - b;
    return length(max(d, 0.)) + min(max(d.x, d.y), 0.);
}

float sdA(vec2 uv, float ah, float al, float t, bool inner)
{
    uv *= rot(sin((iTime) * 4.) * .1);
 	float a = 0.;
    a = S(ah, al, sdBox(vec2(uv.x + .1, uv.y) * rot(PI * .08), vec2(t, .25 + t)));
    a += S(ah, al, sdBox(vec2(uv.x - .1, uv.y) * rot(-PI * .08), vec2(t, .25 + t)));
    a += S(ah, al, sdBox(vec2(uv.x, uv.y + .05), vec2(.1, t * .8)));
    a = min(a, S(ah, al, sdBox(uv, vec2(.26, .26))));
    if(inner)
    	a = min(a, S(ah, al, sdBox(uv, vec2(.26, .25))));
    return a;
}

float sdB(vec2 uv, float ah, float al, float t, float inner)
{
	uv *= rot(sin((iTime) * 4.) * .1);
    float b = S(ah, al, sdBox(vec2(uv.x + .12, uv.y), vec2(t, .2 + t)));
    b += S(ah, al, abs(sdBox(vec2(uv.x+t+inner, uv.y-.12), vec2(.1, .0001))-.09)-t*.9);
    b += S(ah, al, abs(sdBox(vec2(uv.x+t+inner, uv.y+.12), vec2(.1, .0001))-.09)-t*.9);
    b = min(b, S(ah, al, sdBox(vec2(uv.x - .04, uv.y), vec2(.22, .28))));
    if (inner > 0.)
        b = min(b, S(ah, al, sdBox(vec2(uv.x - .0435, uv.y), vec2(.21, .28))));
    return b; 
}

float sdD(vec2 uv, float ah, float al, float t, float inner)
{
    uv *= rot(sin(iTime * 4.) * .1);
	float d = S(ah, al, sdBox(vec2(uv.x + .12, uv.y), vec2(t, .2 + t)));
    d += S(ah, al, abs(sdBox(vec2(uv.x+t+inner+.06, uv.y), vec2(.1, .0001))-.202)-t);
    d = min(d, S(ah, al, sdBox(vec2(uv.x - .04, uv.y), vec2(.22, .28))));
    if (inner > 0.)
        d = min(d, S(ah, al, sdBox(vec2(uv.x - .07, uv.y), vec2(.236, .26))));
    return d;
}

float sdH(vec2 uv, float ah, float al, float t)
{
    uv *= rot(sin(iTime * 4.) * .1);
	float h = 0.;
    h = S(ah, al, sdBox(vec2(uv.x + .12, uv.y), vec2(t, .2 + t)));
    h += S(ah, al, sdBox(vec2(uv.x - .12, uv.y), vec2(t, .2 + t)));
    h += S(ah, al, sdBox(uv, vec2(.1, t)));
    return h;
}

float sdI(vec2 uv, float ah, float al, float t)
{
    uv *= rot(sin(iTime * 4.) * .1);
    return S(ah, al, sdBox(uv, vec2(t, .2 + t)));
}

float sdP(vec2 uv, float ah, float al, float t, float inner)
{
    uv *= rot(sin(iTime * 4.) * .1);
    float p = S(ah, al, sdBox(vec2(uv.x + .12, uv.y), vec2(t, .2 + t)));
    p += S(ah, al, abs(sdBox(vec2(uv.x+t+inner, uv.y-.106), vec2(.1, .0001))-.1)-t);
    p = min(p, S(ah, al, sdBox(vec2(uv.x - .04, uv.y), vec2(.22, .28))));
    if (inner > 0.)
        p = min(p, S(ah, al, sdBox(vec2(uv.x - .043, uv.y), vec2(.21, .28))));
    return p;
}

float sdR(vec2 uv, float ah, float al, float t, float inner)
{
    uv *= rot(sin(iTime * 4.) * .1);
    float r = S(ah, al, sdBox(vec2(uv.x + .12, uv.y), vec2(t, .2 + t)));
    r += S(ah, al, abs(sdBox(vec2(uv.x+t+inner, uv.y-.106), vec2(.1, .0001))-.1)-t);
    r += S(ah, al, sdBox(vec2(uv.x - .1, uv.y + .18) * rot(-PI * .2), vec2(t, .2)));
    r = min(r, S(ah, al, sdBox(vec2(uv.x - .04, uv.y - .02), vec2(.22, .28))));
    if (inner > 0.)
        r = min(r, S(ah, al, sdBox(vec2(uv.x-.04, uv.y-.02-inner), vec2(.207, .28))));
    return r;
}

float sdT(vec2 uv, float ah, float al, float t, bool inner)
{
    uv *= rot(sin(iTime * 4.) * .1);
	float tt = S(ah, al, sdBox(vec2(uv.x, uv.y + .03), vec2(t, .23)));
    tt += S(ah, al, sdBox(vec2(uv.x, uv.y - .2), vec2(.23, t)));
    if(inner)
        tt = min(tt, S(ah, al, sdBox(uv, vec2(.22, .25))));
    return tt;
}

float sdY(vec2 uv, float ah, float al, float t, bool inner)
{
    uv *= rot(sin(iTime * 4.) * .1);
    float y = S(ah, al, sdBox(vec2(uv.x, uv.y + .14), vec2(t, .12)));
    y += S(ah, al, sdBox(vec2(uv.x + .1, uv.y - .14) * rot(PI * .86), vec2(t, .24)));
    y += S(ah, al, sdBox(vec2(uv.x - .1, uv.y - .14) * rot(-PI * .86), vec2(t, .24)));
    y = min(y, S(ah, al, sdBox(vec2(uv.x, uv.y + .2), vec2(.45))));
    if (inner)
        y = min(y, S(ah, al, sdBox(vec2(uv.x, uv.y + .005), vec2(.3, .245))));
    return y;
}

/***************************** Main function *****************************/

void main()
{
    // Fix coordinates for aspect ratio and scale
    vec2 uv = (fragCoord + fragCoord - iResolution.xy) / iResolution.y * uScale;
    
    vec3 col = drawBackground();

    const vec3 white = vec3(1.0);
    const vec3 shadow = vec3(0.1);
    float shadowStr = .666;
    float topGrad = 0.0;
    float botGrad = 0.0;
    vec2 st = vec2(0);
    float angle = 0.0;
    float spiral = 0.0;

// H
    SETUP_LETTER(uv.x + .76, uv.y - .4, iTime * 2.0, exp(-1.5 * iTime), spiral * 10.0 * sin(angle), spiral * 12.5 * cos(angle));
	col = mix(col, shadow, shadowStr * sdH(st, .06, -.05, .06));
    col = mix(col, white, sdH(st, .015, .005, .06));
    col = mix(col, mix(white, vec3(.006, .08, .99), topGrad), 
              C(sdH(st, .015, .005, .048)));

// A
    SETUP_LETTER(uv.x + .37, uv.y - .4, iTime * 3.0, exp(-0.6 * iTime), spiral * cos(angle), spiral * 2.2 * cos(angle));
    col = mix(col, shadow, shadowStr * C(sdA(st, .05, -.05, .05, false)));
    col = mix(col, white, sdA(st, .015, .005, .055, false));
    col = mix(col, mix(white, vec3(.99, .001, .005), topGrad), 
              sdA(st, .015, .005, .044, true));

// P
    SETUP_LETTER(uv.x, uv.y - .4, iTime * 2.5, exp(-0.7 * iTime), spiral * -5.5 * sin(angle), spiral * 8.4 * cos(angle));
    col = mix(col, shadow, shadowStr * sdP(st, .06, -.06, .05, 0.));
    col = mix(col, white, sdP(st, .015, .005, .06, 0.));
    col = mix(col, mix(white, vec3(.02, .95, .06), topGrad),
              sdP(st, .015, .005, .046, .01));

// P
    SETUP_LETTER(uv.x - .34, uv.y - .4, iTime * 4.0, exp(-0.35 * iTime), spiral * cos(angle), spiral * sin(angle));
    col = mix(col, shadow, shadowStr * sdP(st, .06, -.06, .05, 0.));
    col = mix(col, white, sdP(st, .015, .005, .06, 0.));
    col = mix(col, mix(white, vec3(.98, .42, .01), topGrad),
              sdP(st, .015, .005, .046, .01));

// Y
    SETUP_LETTER(uv.x - .66, uv.y - .4, iTime * 3.0, exp(-2.0 * iTime), spiral * 9.0 * max(0.0, 1.0 - 0.4 * iTime), spiral * 13.0 * max(0.0, 1.0 - 0.25 * iTime));
    col = mix(col, shadow, shadowStr * sdY(st, .05, -.05, .05, false));
    col = mix(col, white, sdY(st, .015, .005, .06, false));
    col = mix(col, mix(white, vec3(.98, .01, .34), topGrad), 
              sdY(st, .015, .005, .048, true));

// B
    SETUP_LETTER(uv.x + 1.2, uv.y + .4, iTime * 3.0, exp(-2.0 * iTime), spiral * pow(iTime + 2.0, 3) * cos(angle), spiral * pow(iTime + 3.0, 2) * sin(angle));
    col = mix(col, shadow, shadowStr * sdB(st, .06, -.06, .05, 0.));
    col = mix(col, white, sdB(st, .015, .005, .06, 0.));
    col = mix(col, mix(white, vec3(.99, .001, .005), botGrad),
              sdB(st, .015, .005, .046, .01));

// I
    SETUP_LETTER(uv.x + .96, uv.y + .4, iTime * 3.0, exp(-2.0 * iTime), spiral * sin(angle), pow(spiral, 0.35) * 5.0 * sin(angle));
    col = mix(col, shadow, shadowStr * sdI(st, .06, -.06, .06));
    col = mix(col, white, sdI(st, .015, .005, .06));
    col = mix(col, mix(white, vec3(.01, .56, .87), botGrad), 
              sdI(st, .015, .005, .048));

// R
    SETUP_LETTER(uv.x + .71, uv.y + .4, iTime * 2.0, exp(-1.0 * iTime), spiral * 0.5 * cos(angle), spiral * 3.0 * sin(angle));
    col = mix(col, shadow, shadowStr * sdR(st, .06, -.06, .05, .0));
	col = mix(col, white, sdR(st, .015, .005, .06, .0));
    col = mix(col, mix(white, vec3(.48, .005, .76), botGrad),
              sdR(st, .015, .005, .046, .01));

// T
    SETUP_LETTER(uv.x + .32, uv.y + .4, iTime * 1.1, exp(-0.5 * iTime), spiral * 8.0 * sin(angle), spiral * 4.5 * cos(angle));
    col = mix(col, shadow, shadowStr * sdT(st, .06, -.06, .06, false));
    col = mix(col, white, sdT(st, .015, .005, .06, false));
    col = mix(col, mix(white, vec3(.97, .96, .006), botGrad), 
              C(sdT(st, .015, .005, .048, true)));

// H
    SETUP_LETTER(uv.x - .08, uv.y + .4, iTime * 2.4, exp(-0.9 * iTime), spiral * 1.2 * sin(angle), spiral * 2.9 * cos(angle));
    col = mix(col, shadow, shadowStr * sdH(st, .06, -.05, .06));
    col = mix(col, white, sdH(st, .015, .005, .06));
    col = mix(col, mix(white, vec3(.98, .01, .34), botGrad), 
              C(sdH(st, .015, .005, .048)));

// D
    SETUP_LETTER(uv.x - .45, uv.y + .4, iTime * 4.0, exp(-1.5 * iTime), spiral * cos(angle), spiral * sin(angle));
    col = mix(col, shadow, shadowStr * sdD(st, .06, -.06, .05, 0.));
    col = mix(col, white, sdD(st, .015, .005, .06, 0.));
    col = mix(col, mix(white, vec3(.02, .95, .06), botGrad), 
              sdD(st, .015, .005, .046, .01));

// A
    SETUP_LETTER(uv.x - .82, uv.y + .4, iTime * 1.8, exp(-0.5 * iTime), spiral * cos(angle) * sin(angle), spiral * cos(angle));
    col = mix(col, shadow, shadowStr * sdA(st, .05, -.05, .05, false));
    col = mix(col, white, sdA(st, .015, .005, .055, false));
    col = mix(col, mix(white, vec3(.006, .08, .99), botGrad), 
              sdA(st, .015, .005, .044, true));

// Y
    SETUP_LETTER(uv.x - 1.12, uv.y + .4, iTime * 7.0, exp(-3.1 * iTime), spiral * cos(angle), spiral * 8.0 * sin(angle));
    col = mix(col, shadow, shadowStr * sdY(st, .05, -.05, .05, false));
    col = mix(col, white, sdY(st, .015, .005, .06, false));
    col = mix(col, mix(white, vec3(.98, .42, .01), botGrad), 
              sdY(st, .015, .005, .048, true));

    O = vec4(pow(col, vec3(.8)), 1.);
}
