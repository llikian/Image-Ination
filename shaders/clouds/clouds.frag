/***************************************************************************************************
 * @file  clouds.frag
 * @brief Fragment shader for rendering the clouds
 **************************************************************************************************/

#version 420 core

in vec3 cameraPos; // Position de la caméra dans l'espace 3D

out vec4 fragColor; // Couleur finale du fragment

// Paramètres de couleur et de lumière
vec3 skytop = vec3(0.1, 0.5, 0.9); // Couleur du ciel en haut
vec3 light = normalize(vec3(0.1, 0.2, 0.9)); // Direction de la lumière
vec2 cloudrange = vec2(1000.0, -1000.0); // Plage de hauteur des nuages
vec3 cloudHeight = vec3(0.0, -10000.0, 0.0); // Position de base des nuages
// Matrice pour la transformation des coordonnées des nuages
mat3 m = mat3(0.00, 1.60, 1.20, -1.60, 0.72, -0.96, -1.20, -0.96, 1.28);

// Uniformes 
uniform vec2 resolution; // Résolution de l'écran
uniform vec3 cameraFront; // Vecteur de direction de la caméra
uniform vec3 cameraRight; // Vecteur de droite de la caméra
uniform vec3 cameraUp; // Vecteur vers le haut de la caméra
uniform float time; // Temps pour l'animation des nuages

// Définition des types de nuages
struct CloudType {
    float densiteMin; // Densité minimale du nuage
    float densiteMax; // Densité maximale du nuage
    vec3 externColor; // Couleur externe du nuage
    vec3 internColor; // Couleur interne du nuage
};

// Initialisation de différents types de nuages
CloudType cirrus = CloudType(0.5, 1., vec3(1.0, 1.0, 1.0), vec3(0.8, 0.8, 0.8));
CloudType cumulus = CloudType(0.6, 1., vec3(1.0, 1.0, 1.0), vec3(0.8, 0.8, 0.8));
CloudType stratus = CloudType(0.1, 0.8, vec3(0.9, 0.9, 0.9), vec3(0.5, 0.5, 0.5));

// Fonction de hachage pour générer des valeurs pseudo-aléatoires
float hash(float n) {
    return fract(cos(n) * 114514.1919);
}

// Hachage en 3D pour générer des valeurs pseudo-aléatoires
float Hash(in vec3 p)  
{
    p  = fract(p * 0.3199 + 0.152);
    p *= 17.0; // Échelle pour le hachage
    return fract(p.x * p.y * p.z * (p.x + p.y + p.z)); // Retourne une valeur hachée
}

// Fonction de bruit 3D
float noise(in vec3 x) {
    vec3 p = floor(x); // Coordonnées entières
    vec3 f = smoothstep(0.0, 1.0, fract(x)); // Interpolation douce
    float n = p.x + p.y * 10.0 + p.z * 100.0; // Calcul de l'index
    // Interpolation entre les valeurs de hachage pour créer le bruit
    return mix(
        mix(mix(hash(n + 0.0), hash(n + 1.0), f.x),
            mix(hash(n + 10.0), hash(n + 11.0), f.x), f.y),
        mix(mix(hash(n + 100.0), hash(n + 101.0), f.x),
            mix(hash(n + 110.0), hash(n + 111.0), f.x), f.y), f.z);
}

// Fonction de bruit fractal
float fbm(vec3 p) {
    float f = 0.5000 * noise(p); // Premier niveau de bruit
    p = m * p; // Transformation avec la matrice
    f += 0.2500 * noise(p); // Deuxième niveau de bruit
    p = m * p; // Nouvelle transformation
    f += 0.1666 * noise(p); // Troisième niveau de bruit
    p = m * p; // Nouvelle transformation
    f += 0.0834 * noise(p); // Quatrième niveau de bruit
    return f; // Retourne le bruit fractal
}

// Fonction de Voronoi pour générer des motifs
float Voronoi(vec3 x, float scale) {
    vec3 p = floor(x / scale); // Coordonnées de cellule
    vec3 f = fract(x / scale); // Position à l'intérieur de la cellule

    float Distance = 100.0; // Distance maximale initiale
    // Boucle à travers les voisins
    for (int k = -1; k <= 1; k++) {
        for (int j = -1; j <= 1; j++) {
            for (int i = -1; i <= 1; i++) {
                vec3 b = vec3(float(i), float(j), float(k)); // Vecteur de décalage
                vec3 r = vec3(b) - f + Hash(p + b); // Calcul de la position relative
                float d = dot(r, r); // Calcul de la distance au carré

                if (d < Distance) {
                    Distance = d; // Met à jour la distance si elle est plus courte
                }
            }
        }
    }

    return sqrt(Distance); // Retourne la racine carrée de la distance
}

// Fonction principale pour générer les nuages
vec4 cloud(vec4 sum, float densiteMin, float densiteMax, vec3 externColor, vec3 internColor, vec3 direction) {
    vec3 position; // Position dans l'espace 3D
    float alpha, density; // Variables pour alpha et densité
    float stepSize = 100.0; // Taille du pas pour le calcul des nuages

    // Boucle pour traverser la profondeur
    for (float depth = 0.0; depth < 100000.0; depth += stepSize) {
        position = cameraPos + direction * depth + cloudHeight + vec3(time * 100.0, 0.0, 0.0); // Calcul de la position du nuage

        // Vérifie si la position est dans la plage de hauteur des nuages
        if (cloudrange.x > position.y && position.y > cloudrange.y) {
            // Calcul de la densité à partir de Voronoi et du bruit fractal
            density = mix(Voronoi(position, 10000.0), fbm(position * 0.0005), 1.0);
            alpha = smoothstep(densiteMin, densiteMax, density); // Interpolation de l'alpha

            vec3 localcolor = mix(externColor, internColor, alpha); // Couleur locale basée sur l'alpha

            alpha *= (1.0 - sum.a); // Applique la transparence
            sum += vec4(localcolor * alpha, alpha); // Ajoute la couleur au résultat total

            if (sum.a > 0.95) break; // Sort de la boucle si alpha est suffisamment élevé
        }
    }

    return sum; // Retourne la somme des couleurs calculées
}

// Fonction pour mélanger deux types de nuages
CloudType mixCloudType(CloudType a, CloudType b, float t) {
    return CloudType(
        mix(a.densiteMin, b.densiteMin, t), // Interpolation de la densité minimale
        mix(a.densiteMax, b.densiteMax, t), // Interpolation de la densité maximale
        mix(a.externColor, b.externColor, t), // Interpolation de la couleur externe
        mix(a.internColor, b.internColor, t)  // Interpolation de la couleur interne
    );
}

// Fonction pour obtenir le type de nuage actif basé sur le temps
CloudType getActiveCloudType(float time) {
    float cycle = mod(time, 40.0); // Cycle de 40 secondes
    float t;

    if (cycle < 5.0) {
        return cirrus; // Nuages de type cirrus
    } else if (cycle < 10.0) {
        t = (cycle - 5.0) / 5.0; // Interpolation entre cirrus et cumulus
        return mixCloudType(cirrus, cumulus, t);
    } else if(cycle < 20.0){
        t = (cycle - 10.0) / 10.0; // Interpolation entre cumulus et stratus
        return mixCloudType(cumulus, stratus, t);
    }
    else{
        t = (cycle - 20.0) / 20.0; // Interpolation entre stratus et cirrus
        return mixCloudType(stratus, cirrus, t);
    }
}

void main() {
    vec2 uv = (2.0 * gl_FragCoord.xy - resolution) / resolution.y; // Calcul des coordonnées UV normalisées
    float focalLength = 2.5f; // Longueur focale pour la caméra
    vec3 direction = mat3(cameraRight, cameraUp, cameraFront) * normalize(vec3(uv, focalLength)); // Direction de la caméra

    vec4 sum = vec4(0.0); // Initialisation de la somme des couleurs
    CloudType activeCloud = getActiveCloudType(time); // Obtient le type de nuage actif

    // Génère les nuages en fonction du type actif
    sum = cloud(
        sum,
        activeCloud.densiteMin,
        activeCloud.densiteMax,
        activeCloud.externColor,
        activeCloud.internColor,
        direction 
    );

    // Ajustement de l'alpha et des couleurs
    float alpha = smoothstep(0.4, 1.0, sum.a); // Ajustement de l'alpha
    sum.rgb /= sum.a + 0.0001; // Normalisation des couleurs par alpha
    float sun = clamp(dot(direction, light), 0.0, 1.0); // Calcul de l'éclairement solaire
    vec3 color = 0.8 * skytop; // Couleur de base
    color += vec3(1.0, 1.0, 1.0) * pow(sun, 350.0); // Ajout de la lumière directe
    color += 0.4 * vec3(1.0, 1.0, 1.0) * pow(sun, 2.0); // Ajout d'une lumière diffuse
    sum.rgb -= 0.6 * vec3(0.8, 0.8, 0.8) * pow(sun, 13.0) * alpha; // Ombres sur les nuages
    sum.rgb += 0.2 * vec3(1.0, 1.0, 1.0) * pow(sun, 5.0) * (1.0 - alpha); // Lumière sur les nuages

    // Ajustement final des couleurs basé sur la distance
    float depthFactor = clamp(length(direction), 0.0, 1.0); // Facteur de profondeur
    color = mix(color, sum.rgb, sum.a * depthFactor); // Mélange des couleurs

    fragColor = vec4(color, 1.0); // Définition de la couleur finale du fragment
}
