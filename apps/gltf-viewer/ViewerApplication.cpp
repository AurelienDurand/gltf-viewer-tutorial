#include "ViewerApplication.hpp"

#include <iostream>
#include <numeric>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>

#include "utils/cameras.hpp"
#include "utils/gltf.hpp" // getLocalToworldMatrix
#include "utils/images.hpp" //renderToImage
#include <stb_image_write.h>

const GLuint VERTEX_ATTRIB_POSITION_IDX = 0;
const GLuint VERTEX_ATTRIB_NORMAL_IDX = 1;
const GLuint VERTEX_ATTRIB_TEXCOORD0_IDX = 2;

///Solution du prof
//std::vector<GLuint> ViewerApplication::createBufferObjects(
//    const tinygltf::Model &model)
//{
//  std::vector<GLuint> bufferObjects(model.buffers.size(), 0);
//
//  glGenBuffers(GLsizei(model.buffers.size()), bufferObjects.data());
//  for (size_t i = 0; i < model.buffers.size(); ++i) {
//    glBindBuffer(GL_ARRAY_BUFFER, bufferObjects[i]);
//    glBufferStorage(GL_ARRAY_BUFFER, model.buffers[i].data.size(),
//        model.buffers[i].data.data(), 0);
//  }
//  glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//  return bufferObjects;
//}
//
//std::vector<GLuint> ViewerApplication::createVertexArrayObjects(
//    const tinygltf::Model &model, const std::vector<GLuint> &bufferObjects,
//    std::vector<VaoRange> &meshToVertexArrays)
//{
//  std::vector<GLuint> vertexArrayObjects; // We don't know the size yet
//
//  // For each mesh of model we keep its range of VAOs
//  meshToVertexArrays.resize(model.meshes.size());
//
//  const GLuint VERTEX_ATTRIB_POSITION_IDX = 0;
//  const GLuint VERTEX_ATTRIB_NORMAL_IDX = 1;
//  const GLuint VERTEX_ATTRIB_TEXCOORD0_IDX = 2;
//
//  for (size_t i = 0; i < model.meshes.size(); ++i) {
//    const auto &mesh = model.meshes[i];
//
//    auto &vaoRange = meshToVertexArrays[i];
//    vaoRange.begin =
//        GLsizei(vertexArrayObjects.size()); // Range for this mesh will be at
//                                            // the end of vertexArrayObjects
//    vaoRange.count =
//        GLsizei(mesh.primitives.size()); // One VAO for each primitive
//
//    // Add enough elements to store our VAOs identifiers
//    vertexArrayObjects.resize(
//        vertexArrayObjects.size() + mesh.primitives.size());
//
//    glGenVertexArrays(vaoRange.count, &vertexArrayObjects[vaoRange.begin]);
//    for (size_t pIdx = 0; pIdx < mesh.primitives.size(); ++pIdx) {
//      const auto vao = vertexArrayObjects[vaoRange.begin + pIdx];
//      const auto &primitive = mesh.primitives[pIdx];
//      glBindVertexArray(vao);
//      { // POSITION attribute
//        // scope, so we can declare const variable with the same name on each
//        // scope
//        const auto iterator = primitive.attributes.find("POSITION");
//        if (iterator != end(primitive.attributes)) {
//          const auto accessorIdx = (*iterator).second;
//          const auto &accessor = model.accessors[accessorIdx];
//          const auto &bufferView = model.bufferViews[accessor.bufferView];
//          const auto bufferIdx = bufferView.buffer;
//
//          glEnableVertexAttribArray(VERTEX_ATTRIB_POSITION_IDX);
//          assert(GL_ARRAY_BUFFER == bufferView.target);
//          // Theorically we could also use bufferView.target, but it is safer
//          // Here it is important to know that the next call
//          // (glVertexAttribPointer) use what is currently bound
//          glBindBuffer(GL_ARRAY_BUFFER, bufferObjects[bufferIdx]);
//
//          // tinygltf converts strings type like "VEC3, "VEC2" to the number of
//          // components, stored in accessor.type
//          const auto byteOffset = accessor.byteOffset + bufferView.byteOffset;
//          glVertexAttribPointer(VERTEX_ATTRIB_POSITION_IDX, accessor.type,
//              accessor.componentType, GL_FALSE, GLsizei(bufferView.byteStride),
//              (const GLvoid *)byteOffset);
//        }
//      }
//      // todo Refactor to remove code duplication (loop over "POSITION",
//      // "NORMAL" and their corresponding VERTEX_ATTRIB_*)
//      { // NORMAL attribute
//        const auto iterator = primitive.attributes.find("NORMAL");
//        if (iterator != end(primitive.attributes)) {
//          const auto accessorIdx = (*iterator).second;
//          const auto &accessor = model.accessors[accessorIdx];
//          const auto &bufferView = model.bufferViews[accessor.bufferView];
//          const auto bufferIdx = bufferView.buffer;
//
//          glEnableVertexAttribArray(VERTEX_ATTRIB_NORMAL_IDX);
//          assert(GL_ARRAY_BUFFER == bufferView.target);
//          glBindBuffer(GL_ARRAY_BUFFER, bufferObjects[bufferIdx]);
//          glVertexAttribPointer(VERTEX_ATTRIB_NORMAL_IDX, accessor.type,
//              accessor.componentType, GL_FALSE, GLsizei(bufferView.byteStride),
//              (const GLvoid *)(accessor.byteOffset + bufferView.byteOffset));
//        }
//      }
//      { // TEXCOORD_0 attribute
//        const auto iterator = primitive.attributes.find("TEXCOORD_0");
//        if (iterator != end(primitive.attributes)) {
//          const auto accessorIdx = (*iterator).second;
//          const auto &accessor = model.accessors[accessorIdx];
//          const auto &bufferView = model.bufferViews[accessor.bufferView];
//          const auto bufferIdx = bufferView.buffer;
//
//          glEnableVertexAttribArray(VERTEX_ATTRIB_TEXCOORD0_IDX);
//          assert(GL_ARRAY_BUFFER == bufferView.target);
//          glBindBuffer(GL_ARRAY_BUFFER, bufferObjects[bufferIdx]);
//          glVertexAttribPointer(VERTEX_ATTRIB_TEXCOORD0_IDX, accessor.type,
//              accessor.componentType, GL_FALSE, GLsizei(bufferView.byteStride),
//              (const GLvoid *)(accessor.byteOffset + bufferView.byteOffset));
//        }
//      }
//      // Index array if defined
//      if (primitive.indices >= 0) {
//        const auto accessorIdx = primitive.indices;
//        const auto &accessor = model.accessors[accessorIdx];
//        const auto &bufferView = model.bufferViews[accessor.bufferView];
//        const auto bufferIdx = bufferView.buffer;
//
//        assert(GL_ELEMENT_ARRAY_BUFFER == bufferView.target);
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
//            bufferObjects[bufferIdx]); // Binding the index buffer to
//                                       // GL_ELEMENT_ARRAY_BUFFER while the VAO
//                                       // is bound is enough to tell OpenGL we
//                                       // want to use that index buffer for that
//                                       // VAO
//      }
//    }
//  }
//  glBindVertexArray(0);
//
//  std::clog << "Number of VAOs: " << vertexArrayObjects.size() << std::endl;
//
//  return vertexArrayObjects;
//}


bool ViewerApplication::loadGltfFile(tinygltf::Model & model)
{
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;
    std::cout << "Chargement  Gltf file "  << std::endl;
    bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, m_gltfFilePath.string());
    //bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, argv[1]); // for binary glTF(.glb)

    if (!warn.empty()) {
        std::cerr << "Warn: %s\n" <<  warn.c_str()<< std::endl;
    }

    if (!err.empty()) {
        std::cerr << "Err: %s\n" << err.c_str()<< std::endl;
    }

    if (!ret) {
        std::cerr << "Echec chargement du glTF\n" << std::endl;
        return false;
    }
    return true;
}


std::vector<GLuint> ViewerApplication::createBufferObjects( const tinygltf::Model &model)
{
    std::vector<GLuint> bufferloc(model.buffers.size(),0);
    //GLuint vbo = 0;
    glGenBuffers((GLsizei)model.buffers.size(),bufferloc.data());
    for(size_t i =0; i< model.buffers.size(); i++) {
        glBindBuffer(GL_ARRAY_BUFFER,bufferloc[i]);
        glBufferStorage(GL_ARRAY_BUFFER, model.buffers[i].data.size(),model.buffers[i].data.data(), 0) ;
    }
//    model.buffers[bufferIdx].data;
    glBindBuffer(GL_ARRAY_BUFFER,0);
    return bufferloc;
}

std::vector<GLuint> ViewerApplication::createVertexArrayObjects( const tinygltf::Model &model,
        const std::vector<GLuint> &bufferObjects,
        std::vector<VaoRange> & meshIndexToVaoRange)
{
    std::vector<GLuint> vao;
    //vao.resize(model.meshes.size());
    /*
    const auto vaoOffset = vao.size();
    vao.resize(vaoOffset + model.meshes[meshIdx].primitives.size());
    meshIndexToVaoRange.push_back(VaoRange{vaoOffset, model.meshes[meshIdx].primitives.size()}); // Will be used during rendering*/
    GLuint tab[3] = {VERTEX_ATTRIB_POSITION_IDX,VERTEX_ATTRIB_NORMAL_IDX,VERTEX_ATTRIB_TEXCOORD0_IDX};
    for(int meshIdx=0 ; meshIdx < model.meshes.size(); meshIdx++ ) {
        const auto &meshloc = model.meshes[meshIdx];
        const auto vaoOffset = vao.size();
        vao.resize(vaoOffset + meshloc.primitives.size());
        VaoRange vao_Range;
        vao_Range.begin = (GLsizei)vaoOffset;
        vao_Range.count = (GLsizei)meshloc.primitives.size();
        //meshIndexToVaoRange.push_back(VaoRange{(GLsizei)vaoOffset, (GLsizei)meshloc.primitives.size()});
        meshIndexToVaoRange.push_back(vao_Range);
        glGenVertexArrays(vao_Range.count, &vao[vaoOffset]);
        for(auto prIdx = 0 ; prIdx < meshloc.primitives.size(); prIdx++) {
            const auto vaoprimIdx = vao[vaoOffset+prIdx];
            const auto &primitive = meshloc.primitives[prIdx];
            glBindVertexArray(vaoprimIdx);

            glEnableVertexAttribArray(VERTEX_ATTRIB_NORMAL_IDX);//1
            glEnableVertexAttribArray(VERTEX_ATTRIB_TEXCOORD0_IDX);//2
            // Position Attribut
            for(int i = 0 ; i<3 ; i++) {
                const GLuint vertex_attrib_idx =tab[i];
                char *attribut;
                switch(i) {
                case 0:
                    attribut= "POSITION";
                    //vertex_attrib_idx = VERTEX_ATTRIB_POSITION_IDX;
                    break;
                case 1:
                    attribut = "NORMAL";
                    //vertex_attrib_idx = VERTEX_ATTRIB_NORMAL_IDX;
                    break;
                case 2:
                    attribut = "TEXCOORD_0";
                    //vertex_attrib_idx = VERTEX_ATTRIB_TEXCOORD0_IDX;
                    break;
                }

                {
                    // I'm opening a scope because I want to reuse the variable iterator in the code for NORMAL and TEXCOORD_0
                    const auto iterator = primitive.attributes.find(attribut);
                    if (iterator != end(primitive.attributes)) { // If "POSITION" has been found in the map
                        // (*iterator).first is the key "POSITION", (*iterator).second is the value, ie. the index of the accessor for this attribute
                        const auto accessorIdx = (*iterator).second;
                        const auto &accessor = model.accessors[accessorIdx];  // TODO get the correct tinygltf::Accessor from model.accessors
                        const auto &bufferView = model.bufferViews[accessor.bufferView];// TODO get the correct tinygltf::BufferView from model.bufferViews. You need to use the accessor
                        const auto bufferIdx = bufferView.buffer;// TODO get the index of the buffer used by the bufferView (you need to use it)
                        const auto bufferObject = bufferObjects[bufferIdx];// TODO get the correct buffer object from the buffer index
                        // TODO Enable the vertex attrib array corresponding to POSITION with glEnableVertexAttribArray (you need to use VERTEX_ATTRIB_POSITION_IDX which has to be defined at the top of the cpp file)
                        glEnableVertexAttribArray(vertex_attrib_idx); //0
                        // TODO Bind the buffer object to GL_ARRAY_BUFFER
                        glBindBuffer(GL_ARRAY_BUFFER,bufferObject);

                        const auto byteOffset = accessor.byteOffset + bufferView.byteOffset ;// TODO Compute the total byte offset using the accessor and the buffer view
                        // TODO Call glVertexAttribPointer with the correct arguments.
                        glVertexAttribPointer(vertex_attrib_idx, accessor.type,
                                              accessor.componentType,GL_FALSE,GLsizei(bufferView.byteStride),
                                              (const GLvoid*)(byteOffset));

                        // Remember size is obtained with accessor.type, type is obtained with accessor.componentType.
                        // The stride /* Taille en octet d'un vertex complet entre chaque attribut position */
                        //  is obtained in the bufferView, normalized is always GL_FALSE, and pointer is the byteOffset (don't forget the cast).


//                         glVertexAttribPointer(0, /* Indice attribut VERTEX POSITION*/
//                          3,                    /* Nombre de composantes -> 3 car en 3D un point = 3 coord*/
//                          GL_FLOAT,             /* Type d'une composante */
//                          GL_FALSE,             /* Pas de normalisation */
//                          sizeof(ShapeVertex) , /* Taille en octet d'un vertex complet entre chaque attribut position */
//                          (const GLvoid*)(offsetof(ShapeVertex, position)));   // position pointeur dans la structure
                    }
                }
            }
            // Index array if defined
            if (primitive.indices >= 0) {
                const auto accessorIdx = primitive.indices;
                const auto &accessor = model.accessors[accessorIdx];
                const auto &bufferView = model.bufferViews[accessor.bufferView];
                const auto bufferIdx = bufferView.buffer;
                assert(GL_ELEMENT_ARRAY_BUFFER == bufferView.target);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                             bufferObjects[bufferIdx]); // Binding the index buffer to
                // GL_ELEMENT_ARRAY_BUFFER while the VAO
                // is bound is enough to tell OpenGL we
                // want to use that index buffer for that
                // VAO
            }
        }
    }
    glBindVertexArray(0);
    std::clog << "Number of VAOs: " << vao.size() << std::endl;
    return vao;

}


void keyCallback(
    GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
        glfwSetWindowShouldClose(window, 1);
    }
}

int ViewerApplication::run()
{
    // Loader shaders
    const auto glslProgram =
        compileProgram({m_ShadersRootPath / m_AppName / m_vertexShader,
                        m_ShadersRootPath / m_AppName / m_fragmentShader});

    const auto modelViewProjMatrixLocation =
        glGetUniformLocation(glslProgram.glId(), "uModelViewProjMatrix");
    const auto modelViewMatrixLocation =
        glGetUniformLocation(glslProgram.glId(), "uModelViewMatrix");
    const auto normalMatrixLocation =
        glGetUniformLocation(glslProgram.glId(), "uNormalMatrix");
    const auto LigthDirectionLocation =
        glGetUniformLocation(glslProgram.glId(), "uWi");
    const auto LigthIntensityLocation =
        glGetUniformLocation(glslProgram.glId(), "uLi");
    const auto BaseColorFactorLocation =
        glGetUniformLocation(glslProgram.glId(), "uBaseColorFactor");

    tinygltf::Model model;
    // TODO Loading the glTF file
    if (!loadGltfFile(model)) {
        return -1;

    }
    glm::vec3 bboxMin, bboxMax;
    computeSceneBounds(model, bboxMin, bboxMax);

    // Build projection matrix
//    auto maxDistance = 500.f; // TODO use scene bounds instead to compute this
//    maxDistance = maxDistance > 0.f ? maxDistance : 100.f;
    const auto diag = bboxMax -bboxMin; // On Prend le vecteur entre les bornes min et max de la box
    auto maxDistance = glm::length(diag);
    maxDistance = maxDistance > 0.f ? maxDistance : 100.f;

    const auto projMatrix =
        glm::perspective(70.f, float(m_nWindowWidth) / m_nWindowHeight,
                         0.001f * maxDistance, 1.5f * maxDistance);

    // TODO Implement a new CameraController model and use it instead. Propose the
    // choice from the GUI

std::unique_ptr<CameraController> cameraController = std::make_unique<FirstPersonCameraController> (
        m_GLFWHandle.window(), 0.1f * maxDistance);

  /* FirstPersonCameraController cameraController_FPS{
        m_GLFWHandle.window(), 0.1f * maxDistance};*/
    if (m_hasUserCamera) {
     cameraController->setCamera(m_userCamera);
    } else {
        // TODO Use scene bounds to compute a better default camera
//        cameraController.setCamera(
//            Camera{glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0)});
//          On prend le millieu de la box comme point centre de la caméra
            const auto center = 0.5f *(bboxMax +bboxMin);
            const auto up = glm::vec3(0,1,0);
            // il faut tester l'axe z (profondeur pour savoir si on est en 2d/3d)
            const auto eye = diag.z > 0 ? center + diag : center + 2.f * glm::cross(diag, up);
            cameraController->setCamera(Camera{eye, center, up});
    }
/// Création des vecteur Wi et Li pour la lumière
    glm::vec3 lightDirection(1,1,1), lightIntensity(1,1,1);
     bool lightFromCamera = false;
	//TrackBall
    /*TrackballCameraController cameraController_trackball{
        m_GLFWHandle.window()};
    if (m_hasUserCamera) {
      cameraController_trackball.setCamera(m_userCamera);
    } else {
      // TODO Use scene bounds to compute a better default camera
      //        cameraController.setCamera(
      //            Camera{glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0,
      //            1, 0)});
      const auto center = 0.5f * (bboxMax + bboxMin);
      const auto up = glm::vec3(0, 1, 0);
      // il faut tester l'axe z (profondeur pour savoir si on est en 2d/3d)
      const auto eye =
          diag.z > 0 ? center + diag : center + 2.f * glm::cross(diag, up);
      cameraController_trackball.setCamera(Camera{eye, center, up});
    }
*/
    //  Creation of Buffer Objects
    const std::vector<GLuint> BufferObjectsvector = createBufferObjects(model);
    // const auto vbos = createBufferObjects(model);

    //  Creation of Vertex Array Objects
    std::vector<VaoRange> meshToVertexArrays;
    const auto vao = createVertexArrayObjects(model, BufferObjectsvector, meshToVertexArrays);

    // Setup OpenGL state for rendering
    glEnable(GL_DEPTH_TEST);
    glslProgram.use();

    // Lambda function to draw the scene
    //[&] captures all symbols by reference
    const auto drawScene = [&](const Camera &camera) {
        glViewport(0, 0, m_nWindowWidth, m_nWindowHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const auto viewMatrix = camera.getViewMatrix();
//        std::cout << LigthDirectionLocation << " and " << LigthIntensityLocation << std::endl;
        if(lightFromCamera){
             glUniform3f(LigthDirectionLocation, 0,0, 1);
        } else if (LigthDirectionLocation >= 0) {
              const auto lightDirectionInViewSpace =
                  glm::normalize(glm::vec3(viewMatrix * glm::vec4(lightDirection, 0.)));
                  glUniform3f(LigthDirectionLocation, lightDirectionInViewSpace[0],
                  lightDirectionInViewSpace[1], lightDirectionInViewSpace[2]);
        }

        if (LigthIntensityLocation >= 0) { // on check le retour des variable uniform qui doivent être strictement supérieur à 0
                // si <0 alors la variable n'est soit pas défini dans le shader soit non utilisée
              glUniform3f(LigthIntensityLocation, lightIntensity[0], lightIntensity[1],
                  lightIntensity[2]);
        }

        // The recursive function that should draw a node
        // We use a std::function because a simple lambda cannot be recursive
        const std::function<void(int, const glm::mat4 &)> drawNode =
        [&](int nodeIdx, const glm::mat4 &parentMatrix) {
            // TODO The drawNode function
            // Model Matrix == localToWorld matrix
            // View Matrix == worldToView matrix
            // modelViewMatrix = view * model
            // modelViewProjectionMatrix = projection * modelViewMatrix
            // normalMatrix = transpose(inverse(mvMatrix)) --> envoyer au shader donc aux GPU avec glUniformMatrix4fv
            const auto &node= model.nodes[nodeIdx];
            const glm::mat4 modelMatrix  = getLocalToWorldMatrix(node,parentMatrix);

            if( node.mesh >= 0) {
                const glm::mat4 MVMatrix = viewMatrix* modelMatrix ;
                const glm::mat4 MVProjectionMatrix = projMatrix * MVMatrix;
                // Normal matrix is necessary to maintain normal vectors
                // orthogonal to tangent vectors
                // https://www.lighthouse3d.com/tutorials/glsl-12-tutorial/the-normal-matrix/
                const glm::mat4 NormalMatrix =  glm::transpose(glm::inverse(MVMatrix));
                glUniformMatrix4fv(modelViewMatrixLocation,1,GL_FALSE,glm::value_ptr(MVMatrix));
                glUniformMatrix4fv(modelViewProjMatrixLocation,1,GL_FALSE,glm::value_ptr(MVProjectionMatrix));
                glUniformMatrix4fv(normalMatrixLocation,1,GL_FALSE,glm::value_ptr(NormalMatrix));
                const auto &meshloc = model.meshes[node.mesh];
                const auto &vaoRange = meshToVertexArrays[node.mesh];

                for(size_t prIdx = 0; prIdx < meshloc.primitives.size(); prIdx++) {
                    const auto vaoloc = vao[vaoRange.begin+prIdx];
                    const auto &primloc = meshloc.primitives[prIdx];
                    glBindVertexArray(vaoloc);
                    if(primloc.indices >= 0) {
                        const auto accessorIdx = primloc.indices;
                        const auto &accessor = model.accessors[accessorIdx];
                        const auto &bufferView = model.bufferViews[accessor.bufferView];
                        const auto byteOffset = accessor.byteOffset + bufferView.byteOffset ;
                        const auto &primMaterialIdx = primloc.material;
                        const auto &accessMaterial = model.materials[primMaterialIdx];
                        //std::cout << primMaterial << std::endl;
                        const auto &pbrMetallicRoughness = accessMaterial.pbrMetallicRoughness;
                        const auto meshebaseColor = pbrMetallicRoughness.baseColorFactor;
                        glm::vec4 meshbaseColorvec4 = glm::vec4(meshebaseColor[0],meshebaseColor[1],meshebaseColor[2],meshebaseColor[3]);
                        glUniform4fv(BaseColorFactorLocation,1 ,glm::value_ptr(meshbaseColorvec4) );

                        glDrawElements(primloc.mode,(GLsizei)accessor.count,accessor.componentType,(const GLvoid*)(byteOffset));
                    } else {
                        // we need the number of vertex to rend
                        const auto accessorIdx = (*begin(primloc.attributes)).second;
                        const auto &accessor = model.accessors[accessorIdx];
                        glDrawArrays(primloc.mode,0,(GLsizei)accessor.count);
                    }
                    glBindVertexArray(0);
                }
            }
            for( auto it_child: node.children) {
                drawNode(it_child,modelMatrix );
            }

        };

        // Draw the scene referenced by gltf file
        if (model.defaultScene >= 0) {
            // TODO Draw all nodes
            for(const auto it_node: model.scenes[model.defaultScene].nodes) {
                drawNode(it_node,glm::mat4(1));
            }
        }
    };
    /*void renderToImage(size_t width, size_t height, size_t numComponents,
    unsigned char *outPixels, std::function<void()> drawScene); */
    if(!m_OutputPath.empty()){
       const int numComponents = 3;
       std::vector<unsigned char> pixels(m_nWindowWidth * m_nWindowHeight * numComponents);
       // [&] means we "capture" all variables from the current scope by reference.
       // It means we can access variables that are defined outside of the lambda.
       // In particular, we can access cameraController and drawScene.
       renderToImage(m_nWindowWidth, m_nWindowHeight, numComponents, pixels.data(), [&]() {
            const auto camera = cameraController->getCamera();
            drawScene(camera);
        });
        // void flipImageYAxis(size_t width, size_t height, size_t numComponent, ComponentType *pixels)
        flipImageYAxis(m_nWindowWidth,m_nWindowHeight, numComponents, pixels.data());
        const auto strPath = m_OutputPath.string();
        stbi_write_png(strPath.c_str(), m_nWindowWidth, m_nWindowHeight, 3, pixels.data(), 0);
        return 0;
    }





	/*std::pair<FirstPersonCameraController, TrackballCameraController>
        cameraController = std::make_pair(
            cameraController_FPS, cameraController_trackball);*/

    int currentcam = 0;
    // Loop until the user closes the window
    for (auto iterationCount = 0u; !m_GLFWHandle.shouldClose();
            ++iterationCount) {
        const auto seconds = glfwGetTime();
        auto camera = cameraController->getCamera();
      /*if (currentcam == 0) {
			const auto camera = cameraController.first.getCamera();
			drawScene(camera);
      } else if(currentcam == 1) {
		    const auto camera = cameraController.second.getCamera();
			drawScene(camera);
	  }*/
      /*const auto camera = currentcam == 0 ? cameraController.first.getCamera()
                                          : cameraController.second.getCamera();*/

	  drawScene(camera);
        // GUI code:
      imguiNewFrame();

        {
            ImGui::Begin("GUI");
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                        1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Text("eye: %.3f %.3f %.3f", camera.eye().x, camera.eye().y,
                            camera.eye().z);
                ImGui::Text("center: %.3f %.3f %.3f", camera.center().x,
                            camera.center().y, camera.center().z);
                ImGui::Text(
                    "up: %.3f %.3f %.3f", camera.up().x, camera.up().y, camera.up().z);

                ImGui::Text("front: %.3f %.3f %.3f", camera.front().x, camera.front().y,
                            camera.front().z);
                ImGui::Text("left: %.3f %.3f %.3f", camera.left().x, camera.left().y,
                            camera.left().z);

                if (ImGui::Button("CLI camera args to clipboard")) {
                    std::stringstream ss;
                    ss << "--lookat " << camera.eye().x << "," << camera.eye().y << ","
                       << camera.eye().z << "," << camera.center().x << ","
                       << camera.center().y << "," << camera.center().z << ","
                       << camera.up().x << "," << camera.up().y << "," << camera.up().z;
                    const auto str = ss.str();
                    glfwSetClipboardString(m_GLFWHandle.window(), str.c_str());
                }
                if (ImGui::Button("Change Camera")) {
                    const auto currentCamera = cameraController->getCamera();
                    if (currentcam == 0) {
                        cameraController = std::make_unique<TrackballCameraController>(
                        m_GLFWHandle.window(), 0.1f * maxDistance);
                        currentcam = 1;
                        const auto center = 0.5f *(bboxMax +bboxMin);
                        const auto up = glm::vec3(0,1,0);
                        // il faut tester l'axe z (profondeur pour savoir si on est en 2d/3d)
                        const auto eye = diag.z > 0 ? center + diag : center + 2.f * glm::cross(diag, up);
                        cameraController->setCamera(Camera{eye, center, up});


                      } else if (currentcam == 1) {
                        cameraController = std::make_unique<FirstPersonCameraController>(
                            m_GLFWHandle.window(), 0.1f * maxDistance);
                        currentcam = 0;
                        cameraController->setCamera(currentCamera);
                      }


				}
                if (currentcam == 0) {
                  ImGui::Text("Current cam : FPS");
				} else if (currentcam == 1) {
                  ImGui::Text("Current cam : Trackball");
				}
				if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen)) {
                    static float lightTheta = 0.f;
                    static float lightPhi = 0.f;

                    if (ImGui::SliderFloat("theta", &lightTheta, 0, glm::pi<float>()) || // bar slide pour changer la valeur de theta et phi
                        ImGui::SliderFloat("phi", &lightPhi, 0, 2.f * glm::pi<float>())) {
                      const auto sinPhi = glm::sin(lightPhi);
                      const auto cosPhi = glm::cos(lightPhi);
                      const auto sinTheta = glm::sin(lightTheta);
                      const auto cosTheta = glm::cos(lightTheta);
                      lightDirection =
                          glm::vec3(sinTheta * cosPhi, cosTheta, sinTheta * sinPhi);
                    }

                    static glm::vec3 lightColor(1.f, 1.f, 1.f);
                    static float lightIntensityFactor = 1.f;
                    if (ImGui::ColorEdit3("color", (float *)&lightColor) ||
                        ImGui::SliderFloat("intensity", &lightIntensityFactor, 0, 25.f)/*||
                        ImGui::InputFloat("intensity", &lightIntensityFactor)*/) {
                            lightIntensity = lightColor * lightIntensityFactor;
                    }
                    ImGui::Checkbox("light from camera", &lightFromCamera);
                   /* if (ImGui::InputFloat("intensity", &lightIntensityFactor)){
                            lightIntensity = lightColor * lightIntensityFactor;
                        }*/
                  }

            }
            ImGui::End();
        }

        imguiRenderFrame();

        glfwPollEvents(); // Poll for and process events

        auto ellapsedTime = glfwGetTime() - seconds;
        auto guiHasFocus =
            ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
        if (!guiHasFocus) {
                cameraController->update(float(ellapsedTime));
          /*if (currentcam == 0) {
            const auto camera =
                cameraController.first.update(float(ellapsedTime));

          } else if (currentcam == 1) {
                cameraController.second.update(float(ellapsedTime));
          }*/
        }

        m_GLFWHandle.swapBuffers(); // Swap front and back buffers
    }

    // TODO clean up allocated GL data

    return 0;
}

ViewerApplication::ViewerApplication(const fs::path &appPath, uint32_t width,
                                     uint32_t height, const fs::path &gltfFile,
                                     const std::vector<float> &lookatArgs, const std::string &vertexShader,
                                     const std::string &fragmentShader, const fs::path &output) :
    m_nWindowWidth(width),
    m_nWindowHeight(height),
    m_AppPath{appPath},
    m_AppName{m_AppPath.stem().string()},
    m_ImGuiIniFilename{m_AppName + ".imgui.ini"},
    m_ShadersRootPath{m_AppPath.parent_path() / "shaders"},
    m_gltfFilePath{gltfFile},
    m_OutputPath{output}
{
    if (!lookatArgs.empty()) {
        m_hasUserCamera = true;
        m_userCamera =
            Camera{glm::vec3(lookatArgs[0], lookatArgs[1], lookatArgs[2]),
                   glm::vec3(lookatArgs[3], lookatArgs[4], lookatArgs[5]),
                   glm::vec3(lookatArgs[6], lookatArgs[7], lookatArgs[8])};
    }

    if (!vertexShader.empty()) {
        m_vertexShader = vertexShader;
    }

    if (!fragmentShader.empty()) {
        m_fragmentShader = fragmentShader;
    }

    ImGui::GetIO().IniFilename =
        m_ImGuiIniFilename.c_str(); // At exit, ImGUI will store its windows
    // positions in this file

    glfwSetKeyCallback(m_GLFWHandle.window(), keyCallback);

    printGLVersion();
}
