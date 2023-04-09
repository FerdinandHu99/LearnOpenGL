#include "hfmesh.h"

HFMesh::HFMesh(QOpenGLShaderProgram* shaderProgram, QOpenGLContext* OpenGLContext, QObject *parent) : QObject(parent),
    m_VBO(QOpenGLBuffer::VertexBuffer), m_EBO(QOpenGLBuffer::IndexBuffer),
    m_shaderProgram(shaderProgram), m_OpenGLContext(OpenGLContext)
{

}

void HFMesh::loadMeshData(QVector<Vertex> vertices, QVector<unsigned int> indices, QVector<Texture *> textures)
{
    m_vertices = vertices;
    m_indices = indices;
    m_textures = textures;
    initMesh();
}

void HFMesh::draw()
{
    QOpenGLFunctions* OpenGLFunctions = m_OpenGLContext->functions();

    m_VAO.bind();
    m_shaderProgram->bind();
    // 处理纹理数据
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;

    // 设置贴图
    for (unsigned int i = 0; i < m_textures.size(); i++) {
        OpenGLFunctions->glActiveTexture(GL_TEXTURE0 + i);
        QString number;
        QString typeName = m_textures[i]->type;
        if (typeName == "texture_diffuse") {
            number = QString::number(diffuseNr++);
        } else if (typeName == "texture_specular") {
            number = QString::number(specularNr++);
        }
        m_textures[i]->texture.bind();
        m_shaderProgram->setUniformValue((typeName+number).toStdString().c_str(), i);
    }

    // 设置模型变化矩阵（相当于设置模型的世界坐标）
    //m_shaderProgram->setUniformValue("model", modelMatrix);

    // 绘制网格
    OpenGLFunctions->glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);

    /* 将所有纹理清除绑定 */
    for (unsigned int i = 0; i < m_textures.size(); i++) {
        OpenGLFunctions->glActiveTexture(GL_TEXTURE0 + i);
        OpenGLFunctions->glBindTexture(GL_TEXTURE_2D, 0);
    }

    m_VAO.release();
    m_shaderProgram->release();
}

void HFMesh::initMesh()
{
    m_VAO.create();
    m_VBO.create();
    m_EBO.create();

    m_VAO.bind();
    m_VBO.bind();
    m_VBO.allocate(m_vertices.data(), m_vertices.size() * sizeof (Vertex));
    m_VBO.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_EBO.bind();
    m_EBO.allocate(m_indices.data(), m_indices.size() * sizeof (unsigned int));
    m_EBO.setUsagePattern(QOpenGLBuffer::StaticDraw);

    m_shaderProgram->bind();

    /* 设置顶点坐标 */
    m_shaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, sizeof (Vertex));
    m_shaderProgram->enableAttributeArray(0);
    /* 设置法线向量 */
    m_shaderProgram->setAttributeBuffer(1, GL_FLOAT, offsetof(Vertex, Normal), 3, sizeof (Vertex));
    m_shaderProgram->enableAttributeArray(1);
    /* 设置纹理坐标 */
    m_shaderProgram->setAttributeBuffer(2, GL_FLOAT, offsetof(Vertex, TexCoords), 2, sizeof (Vertex));
    m_shaderProgram->enableAttributeArray(2);

    m_VAO.release();
}
