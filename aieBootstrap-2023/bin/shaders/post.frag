// A simple flat-color shader
#version 410

in vec2 vTexCoord;

uniform sampler2D colorTarget;
uniform int postProcessTarget;
uniform int windowWidth;
uniform int windowHeight;

out vec4 FragColor;

vec4 Defualt(vec2 texCoord) 
{
    return texture(colorTarget, texCoord);
}

vec4 BoxBlur(vec2 texCoord) 
{
    vec2 texel = 1.0 / textureSize(colorTarget, 0);

    vec4 color = texture(colorTarget, texCoord);
    color += texture(colorTarget, texCoord + texel * vec2(-1,  1));
    color += texture(colorTarget, texCoord + texel * vec2(-1,  0));
    color += texture(colorTarget, texCoord + texel * vec2(-1, -1));
    
    color += texture(colorTarget, texCoord + texel * vec2( 0,  1));
    color += texture(colorTarget, texCoord + texel * vec2( 0, -1));

    color += texture(colorTarget, texCoord + texel * vec2( 1,  1));
    color += texture(colorTarget, texCoord + texel * vec2( 1,  0));
    color += texture(colorTarget, texCoord + texel * vec2( 1, -1));

    return color / 9;
}

vec4 Distort(vec2 texCoord)
{
    vec2 mid = vec2(0.5f);
    float distanceFromTheCenter = distance(texCoord, mid);
    vec2 normalizeCoord = normalize(texCoord - mid);
    float bias = distanceFromTheCenter + sin(distanceFromTheCenter * 15) * 0.05f;
    vec2 newCoord = mid + bias * normalizeCoord;
    return texture(colorTarget, newCoord);
}

vec4 EdgeDetection(vec2 texCoord)
{
    float w = 1.0f / windowWidth;
    float h = 1.0f / windowHeight;

    vec4 k[9];
    
    k[0] = texture(colorTarget, texCoord + vec2(-w, -h));
    k[1] = texture(colorTarget, texCoord + vec2( 0, -h));
    k[2] = texture(colorTarget, texCoord + vec2( w, -h));
    k[3] = texture(colorTarget, texCoord + vec2(-w,  0));
    k[4] = texture(colorTarget, texCoord);
    k[5] = texture(colorTarget, texCoord + vec2( w,  0));
    k[6] = texture(colorTarget, texCoord + vec2(-w,  h));
    k[7] = texture(colorTarget, texCoord + vec2( 0,  h));
    k[8] = texture(colorTarget, texCoord + vec2( w,  h));

    vec4 sobelEdgeH = k[2] + (2.0f * k[5]) + k[8] - (k[0] + (2.0f * k[3]) + k[6]);
    vec4 sobelEdgeV = k[0] + (2.0f * k[1]) + k[2] - (k[6] + (2.0f * k[7]) + k[8]);

    vec4 sobel = sqrt((sobelEdgeH * sobelEdgeH) + (sobelEdgeV * sobelEdgeV));

    return vec4(1.0f - sobel.rgb, 1.0f);
}

vec4 Sepia(vec2 texCoord)
{
    vec4 color = texture(colorTarget, texCoord);

    float inputRed = color.r;
    float inputGreen = color.g;
    float inputBlue = color.b;

    color.r = min(255, ((0.393f * inputRed) + (0.769f * inputGreen) + (0.189f * inputBlue))); // Red
    color.g = min(255, ((0.349f * inputRed) + (0.686f * inputGreen) + (0.168f * inputBlue))); // Green
    color.b = min(255, ((0.272f * inputRed) + (0.534f * inputGreen) + (0.131f * inputBlue))); // Blue

    return color;
}

vec4 Scanlines(vec2 texCoord)
{
    vec4 color = texture(colorTarget, texCoord);

    float scanlineIntensity = 0.125f;
    float scanlineCount = 800;
    float scanlineYDelta = sin(200);

    float scanline = sin((texCoord.y - scanlineYDelta) * scanlineCount) * scanlineIntensity;

    color -= scanline;
    return color;
}

vec4 GreyScale(vec2 texCoord)
{
    vec4 color = texture(colorTarget, texCoord);

    float scale = (color.r + color.g + color.b) / 3.0f;

    color.r = scale;
    color.g = scale;
    color.b = scale;

    return color;
}

vec4 Invert(vec2 texCoord)
{
    vec4 color = texture(colorTarget, texCoord);

    color.r = 1 - color.r; // Red
    color.g = 1 - color.g; // Green
    color.b = 1 - color.b; // Blue

    return color;
}

vec4 Pixelizer(vec2 texCoord)
{
	vec2 uv = floor(texCoord * vec2(160.0, 50.0)) / vec2(160.0, 50.0);
	vec4 color = texture(colorTarget, uv);
	color.x = float(int((color.x * 8.0) + 0.5)) / 8.0;
	color.y = float(int((color.y * 8.0) + 0.5)) / 8.0;
	color.z = float(int((color.z * 8.0) + 0.5)) / 8.0;

    return color;
}

vec4 Posterization(vec2 texCoord)
{
    vec4 color = texture(colorTarget, texCoord);
	
	
	return color;
}

vec4 DistanceFog(vec2 texCoord)
{
    return texture(colorTarget, texCoord);
}

vec4 DepthOfField(vec2 texCoord)
{
    return texture(colorTarget, texCoord);
}

vec4 NightVision(vec2 texCoord)
{
    return texture(colorTarget, texCoord);
}

void main()
{
    // This will calculate the texel size
    vec2 texSize = textureSize(colorTarget, 0);
    vec2 texelSize = 1.0f / texSize;
    
    // Then we will adjust the coordinate
    vec2 scale = (texSize - texelSize) / texSize;
    vec2 texCoord = vTexCoord / scale + texelSize * 0.5f;

    switch(postProcessTarget)
    {
        default: // Default
        {
            // Output post process effect here
            FragColor = Defualt(texCoord);
            break;
        }
        case 0: // BoxBlur
        {
            FragColor = BoxBlur(texCoord);
            break;
        }
        case 1: // Distort
        {
            FragColor = Distort(texCoord);
            break;
        }
        case 2: // Edge Detection
        {
            FragColor = EdgeDetection(texCoord);
            break;
        }
        case 3: // Sepia
        {
            FragColor = Sepia(texCoord);
            break;
        }
        case 4: // Scanlines
        {
            FragColor = Scanlines(texCoord);
            break;
        }
        case 5: // Grey Scale
        {
            FragColor = GreyScale(texCoord);
            break;
        }
        case 6: // Invert
        {
            FragColor = Invert(texCoord);
            break;
        }
        case 7: // Pixelizer
        {
            FragColor = Pixelizer(texCoord);
            break;
        }
        case 8: // Posterization
        {
            FragColor = Posterization(texCoord);
            break;
        }
        case 9: // Distance Fog
        {
            FragColor = DistanceFog(texCoord);
            break;
        }
        case 10: // Depth Of Field
        {
            FragColor = DepthOfField(texCoord);
            break;
        }
        case 11: // Night Vision
        {
            FragColor = NightVision(texCoord);
            break;
        }
    }

    
}