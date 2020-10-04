#version 330 
in  vec4 ex_Color; 
in vec3  ex_Pos;
in vec3 ex_Normal;

out vec4 gl_FragColor;


uniform vec3 u_sunLight;
uniform float u_ambientLight;

void main(void) {


	vec3 norm = normalize(ex_Normal);
	vec3 lightDir = normalize(u_sunLight - ex_Pos);  

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * vec3(1,1,1);

	vec3 result = (u_ambientLight + diffuse) * vec3(ex_Color);
	gl_FragColor = ex_Color ;
}