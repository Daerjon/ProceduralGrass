

float4 phong(float3 globalPos, float3 norm, float3 view)
{
    float3 lightPosition = float3(5, 5, 3);
    float3 lightColor = float3(1, 0.9, 0.8)*0.5;
    float3 surfaceColor = float3(1, 0.2, 0.4);
    float ks = 0.7, kd = 0.3, ka = 0.4, m = 12;
    
    view = normalize(view);
    norm = normalize(norm);
    float3 color = surfaceColor * ka; //ambient
	
    float3 lightVec = float3(0, 1, 0);
    float3 halfVec = normalize(view + lightVec);
    color += lightColor * kd * surfaceColor * saturate(dot(norm, lightVec)); //diffuse
    color += lightColor * ks * pow(saturate(dot(norm, halfVec)), m); //specular
    
    lightVec = normalize(lightPosition - globalPos);
    halfVec = normalize(view + lightVec);
    color += lightColor * kd * surfaceColor * saturate(dot(norm, lightVec)); //diffuse
    color += lightColor * ks * pow(saturate(dot(norm, halfVec)), m); //
    
	
    return saturate(float4(color, 1.0f));
}
