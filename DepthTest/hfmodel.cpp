#include "hfmodel.h"

HFModel::HFModel(QOpenGLShaderProgram* shaderProgram, QOpenGLContext* OpenGLContext, QObject *parent) : QObject(parent), m_OpenGLContext(OpenGLContext),
  m_shaderProgram(shaderProgram)
{

}

HFModel::~HFModel()
{
    for (auto mesh : m_meshs) {
        delete mesh;
    }
    for (auto texture: m_texturesLoaded) {
        delete texture;
    }
}

bool HFModel::loadModelFile(QDir filePath)
{
    qDebug() << filePath.path() << endl;
    m_directory = filePath;
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(m_directory.absolutePath().toLocal8Bit(), aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene) {
        qDebug() << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
        return false;
    }
    this->m_directory.cdUp(); // 返回上一级目录
    this->processNode(scene->mRootNode, scene); // 递归处理模型的所有节点
    qDebug() << "总共加载了：" << m_texturesLoaded.size();
    return true;
}

void HFModel::draw()
{
    for (auto mesh : m_meshs) {
        mesh->draw();
    }
}

// 递归处理模型的所有节点
void HFModel::processNode(aiNode *node, const aiScene *scene)
{
    // 添加当前节点中的所有mesh
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        this->m_meshs.push_back(processMesh(mesh, scene));
    }
    // 递归处理该节点的子节点
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        this->processNode(node->mChildren[i], scene);
    }
}

HFMesh* HFModel::processMesh(aiMesh *mesh, const aiScene *scene)
{

    QVector<Vertex>       vertices;   // 顶点数据(顶点坐标、法线向量、纹理坐标)
    QVector<unsigned int> indices;    // 索引数据
    QVector<Texture*>     textures;   // 纹理贴图

    /* 处理当前Mesh的所有顶点数据 */
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        QVector3D vec3D;
        // 位置
        vec3D.setX(mesh->mVertices[i].x);
        vec3D.setY(mesh->mVertices[i].y);
        vec3D.setZ(mesh->mVertices[i].z);
        vertex.Position = vec3D;
        // 法线
        vec3D.setX(mesh->mNormals[i].x);
        vec3D.setY(mesh->mNormals[i].y);
        vec3D.setZ(mesh->mNormals[i].z);
        vertex.Normal = vec3D;
        // 纹理坐标
        if (mesh->mTextureCoords[0]) { // 如果该Mesh顶点数据含有纹理坐标
            QVector2D vec2D;
            vec2D.setX(mesh->mTextureCoords[0][i].x);
            vec2D.setY(mesh->mTextureCoords[0][i].y);
            vertex.TexCoords = vec2D;
        } else {
            vertex.TexCoords = QVector2D(0.0f, 0.0f);
        }
        vertices.push_back(vertex);
    }

    /* 处理当前Mesh的所有索引数据 */
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    /* 处理材质 */
    //qDebug() << "全部的材质数量" << scene->mNumMaterials;
    qDebug() << scene->mMaterials[mesh->mMaterialIndex]->GetName().C_Str();
    if (mesh->mMaterialIndex > 0) { // 如果当前Mesh存在材质

        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        //aiMaterial* material = scene->mMaterials[0];
        qDebug() << "漫反射贴图的数量：" << material->GetTextureCount(aiTextureType_DIFFUSE);
        qDebug() << "镜面反射贴图：" << material->GetTextureCount(aiTextureType_SPECULAR);
        qDebug() << "法线贴图：" << material->GetTextureCount(aiTextureType_NORMALS);

        /* 加载漫反射贴图 */
        if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            //qDebug() << "有漫反射贴图";
            QVector<Texture*> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            for (auto it : diffuseMaps) {
                textures.push_back(it);
            }
        }
        /* 加载镜面反射贴图 */
        if (material->GetTextureCount(aiTextureType_SPECULAR) > 0) {
            //qDebug() << "有镜面反射贴图";
            QVector<Texture*> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            for (auto it : specularMaps) {
                textures.push_back(it);
            }
        }
        /* 加载法线贴图 */
        //material->GetTextureCount(aiTextureType_NORMAL_CAMERA)
        if (material->GetTextureCount(aiTextureType_NORMAL_CAMERA) > 0) {
            qDebug() << "有法线贴图";
            QVector<Texture*> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
            for (auto it : normalMaps) {
                textures.push_back(it);
            }
        }
    }
    HFMesh* hfmesh = new HFMesh(m_shaderProgram, m_OpenGLContext);
    hfmesh->loadMeshData(vertices, indices, textures);
    return hfmesh;
}

/* 加载各种类型（漫发射、镜面、法向）纹理贴图 */
QVector<Texture*> HFModel::loadMaterialTextures(aiMaterial *material, aiTextureType type, QString typeName)
{
    QVector<Texture*> textures;
    for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
    {
        aiString textureName;
        material->GetTexture(type, i, &textureName);
        /* 检查纹理是否在之前加载过 */
        bool skip = false;
        for (unsigned int j = 0; j < m_texturesLoaded.size(); j++) {
            if (m_texturesLoaded[j]->name == QString(textureName.C_Str())) { // 如果已加载纹理容器中有相同的话
                textures.push_back(m_texturesLoaded[j]);  // 从已加载纹理容器中导入
                skip = true;  // 已从文件中加载该贴图
                break;
            }

        }
        if (!skip) {
            Texture* texture = new Texture;
            QImage imgData(m_directory.filePath(textureName.C_Str()));
            if (!imgData.isNull()) {
                texture->name = QString(textureName.C_Str());  // 设置纹理名称
                texture->type = typeName;                      // 设置类型名称
                texture->texture.setData(imgData);             // 为纹理设置图像数据

                textures.push_back(texture);                   // 加入进当前类型的纹理容器中
                m_texturesLoaded.push_back(texture);           // 加入进已加载纹理容器中

            } else {
                qDebug() << "纹理加载失败" << m_directory.filePath(textureName.C_Str());
            }
        }
    }
    return textures;
}

/* 打印模型信息 */
void HFModel::Info()
{
    qDebug() << "Model directory: " << m_directory.path();
    qDebug() << "Model mesh nums: " << m_meshs.size();
    qDebug() << "Model texture nums: " << m_texturesLoaded.size();
}
