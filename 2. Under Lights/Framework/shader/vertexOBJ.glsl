//--- vertex shader: vertex.glvs

#version 330 
// in_Position was bound to attribute index 0 and 
// in_Color was bound to attribute index 1

in  vec3 in_Position; // ��ġ�Ӽ�
in  vec2 in_TexCoord; // ��ġ�Ӽ�
in  vec3 in_Normal; // ��ġ�Ӽ�

out vec4 ex_Color; 
out vec3 ex_Pos;
out vec3 ex_Normal;

//  �� ���� �� �Ž������� ������Դϴ�.  
uniform mat4 u_finalMatrix;
uniform mat4 u_worldMatrix;
uniform vec4 u_color;




void main(void) { 
	gl_Position =  u_finalMatrix * vec4(in_Position,1);

	vec4 worldPosition = u_worldMatrix * vec4(in_Position,1);

	ex_Pos = vec3(u_worldMatrix * vec4(in_Position, 1.0));
    ex_Normal = vec3(u_worldMatrix * vec4(in_Normal, 1.0));
	


	ex_Color = u_color;
}
