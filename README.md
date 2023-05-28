
#  **Grass Real-Time Rendering with Wind**
Author: Kaiyang Yao, Shengyuan Wang

## **Introduction**
In this project, we created a realistic and visually appealing simulation of grass using OpenGL, which has a variety of potential applications both in video games and virtual reality experiences. We utilized the billboarding method, grass simulation, and wind simulation to create a realistic grass landscape.

![](assets/images/grass.gif)

### **Requirements**
To run this project, you need the following:
- C++ compiler
- A tool that generates build files. We use CMake

### **Installation**
To install and run the project, you should following these steps:
```
cd <yourCodeDirectory>
git clone https://github.com/KaiyangYao/COMP465_GrassProject.git
cd COMP465_GrassProject
mkdir build
cd build
cmake-gui ..
```
### **Usage**
Once you have installed and run the project, you can use the following controls:
- WSAD keys to control the directions, space, shift to move camera up or down
- Mouse to look around


## **Implementation Details**

### **Grass Simulation**

To do the billboard rendering, we employed a two-stage approach for billboard rendering. In the first stage, we use triangles to create multiple basic rectangular geometric meshes. In the second stage, we mapped the grass texture onto each rectangle in the mesh to give the impression of individual grass blades.

- **Creating Geometry Mesh**

When creating a geometric mesh for billboard rendering, the first step involves determining the position of each rectangle box within the 3D environment. To accomplish this, we first establish a base point for each rectangle box, which serves as a reference point for generating the boxes. Once the base points are determined, we move on to the next step of generating a rectangle box for each reference point.

To create a rectangle box, we need to establish four additional points that define the dimensions of the box. These points are typically located at the corners of the rectangle box and are calculated based on the width and height of the box. Once we have calculated the position of each corner point, we can add them to the vertex array for rendering purposes.

- **Adding Grass Texture**

The next step in the billboard rendering process is to apply the grass texture overlay onto the boxes to generate the appearance of grass on the screen. However, simply overlaying the texture onto each box is not enough to create a realistic-looking scene. Thus, modifications are required to achieve the desired effect.

One potential solution is to rotate the billboards to create multiple copies of the grass texture with different orientations. By repeating this process, we can generate several grass quads within the same space, each with a different direction. This technique contributes to a more natural-looking environment and helps to enhance the overall realism of the scene.

### **Wind Simulation**

To introduce a wind effect to a grass simulation using a geometry shader, we first define several parameters such as wind strength, frequency, direction, and angle. The wind angle is then calculated based on the current time and frequency, which is then used to generate a transformation matrix for rotating the grass blades.

Next, for each vertex of the grass blades, a random rotation matrix is generated. The vertex is then rotated and scaled by the random matrix, the wind translation matrix, and a random grass size. The resulting world position is transformed into screen space and passed to the fragment shader along with additional information, including texture coordinates, color index, fragment position, and normal.

## **Future Work**
This project has the potential for further development in the future to enhance the realism and interactivity of the grass rendering simulation. Here are some potential areas for investigation:

- **Advanced shadowing animation techniques**:
Currently, the simulation only accounts for the movement of the sun and the resulting shadow patterns. However, shadows cast by nearby objects such as trees or buildings could be simulated, and the interaction between the grass and these shadows could be more accurately modeled. This would create a more immersive and dynamic environment for users to explore.

- **Research into other wind simulation methods**:
Our current approach uses a simple sine wave function to generate wind patterns, but other techniques could be explored, such as fluid dynamics simulations. This would result in a more natural and fluid movement of the grass, enhancing the realism of the simulation.

- **Collision detection with other objects**:
 The grass is currently simulated as if it exists in a vacuum, with no interaction with other objects in the environment. By implementing collision detection algorithms, the grass could interact with other objects such as rocks, trees, and buildings. This would create a more dynamic and interactive environment, opening up new possibilities in video games and other applications.


## **Acknowledgements**

We would like to express our sincere gratitude to Professor Bret Jackson for the support and guidance throughout the project. The help in debugging was particularly appreeciated and played a critical role in achieving our project goals. The valuable insights and feedback provided also helped us to identify areas for improvement, which we will continue to work in the future. Thank you once again for your unwavering support!


## **References**
[1] Kévin Boulanger, Sumanta N Pattanaik, and Kadi Bouatouch. “Rendering grass in real-time with dynamic light sources and shadows”. In: (2006).  
[2] Neyret F. “Synthesizing verdant landscapes using volumetric textures”. In: Euro- graphics Rendering Workshop (1996), pp. 215–224.  
[3] Tan Kim Heok and Daut Daman. “A review on level of detail”. In: Proceedings. International Conference on Computer Graphics, Imaging and Visualization, 2004. CGIV 2004. IEEE. 2004, pp. 70–75.  
[4] Neyret F Meyer A. “Interactive volumetric textures”. In: Eurographics Rendering Workshop (1998), pp. 157–168.  
[5] Balu R. Reeves W. “Approximate and probabilistic algorithms from shading and rendering structured partical systems”. In: Computer Graphics 19.3 (1985), pp. 312–322.  
[6] Changbo Wang et al. “Dynamic modeling and rendering of grass wagging in wind”. In: Computer Animation and Virtual Worlds 16.3-4 (2005), pp. 377–389.  
[7] Vulpinii. 2020. Modeling Grass in 3D space. https://vulpinii.github.io/tutorials/grass-modelisation