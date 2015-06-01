uniform sampler2D source;
uniform sampler2D source3;

uniform vec2 scale;
uniform vec2 screenCenter;
uniform vec2 leftLensCenter;
uniform vec2 rightLensCenter;
uniform vec4 hmdWarpParam;
uniform vec2 scaleIn;


void main(void)
{
	vec2 tcoord=gl_TexCoord[0].st;
	vec2 lensCenter;
    bool isleft=true;
	//changed coordinate adaptation to solve volume stretiching and volume displacement, find another solution?
	if(tcoord[0]<=0.5){
		tcoord[0]+=0.25;
		//tcoord[0]-=1.0;
       // tcoord[0]=tcoord[0]+0.25;
		//gl_FragColor=texture2D(source,tcoord);

	}
	else{
		tcoord[0]-=25;
		//tcoord[0]+=0.25;
       // tcoord[0]=tcoord[0]-0.25;
	//	gl_FragColor=texture2D(source3,tcoord);
        isleft=false;
	}
    vec2 theta = (tcoord - lensCenter) * scaleIn;
	float rSq = theta.x*theta.x + theta.y*theta.y;
	vec2 rvector = theta*(hmdWarpParam.x + hmdWarpParam.y*rSq + hmdWarpParam.z*rSq*rSq + hmdWarpParam.w*rSq*rSq*rSq);
	vec2 tc = (lensCenter + scale * rvector);

	if (any(bvec2(clamp(tc, screenCenter - vec2(0.25,0.25), screenCenter + vec2(0.25,0.25))-tc)))
		gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
	else{
		if(!isleft) gl_FragColor = texture2D(source3, tc);
		else gl_FragColor = texture2D(source, tc);
	}


		
}
