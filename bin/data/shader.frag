uniform sampler2D tex;

//void main(void) {
//    gl_FragColor = texture2D(tex, gl_TexCoord[0]);
//}

    //vec4 pixel_color = texture2D(tex, gl_TexCoord[0].xy);

void main(void) {
    gl_FragColor = texture2D(tex, TexCoord[0]);
}
