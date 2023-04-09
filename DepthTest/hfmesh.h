#ifndef HFMESH_H
#define HFMESH_H

#include <QObject>
//#include <QOpenGLFunctions_3_3_Core>
#include <QVector>
#include <QVector3D>
#include <QVector2D>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
/* 存储顶点数据的结构体：顶点坐标、法线向量、纹理坐标 */
struct Vertex {
    QVector3D Position;
    QVector3D Normal;
    QVector2D TexCoords;
};

/* 存储纹理信息的结构体 */
struct Texture
{
    QString name;             // 材质贴图名称(例如：container_diff.jpg)
    QString type;             // 材质贴图类型（漫反射贴图、镜面反射贴图）
    QOpenGLTexture texture;   // QOpenGL贴图类

    // 结构体构造函数
    Texture() : texture(QOpenGLTexture::Target2D) {
        texture.create();
        texture.setWrapMode(QOpenGLTexture::DirectionS,QOpenGLTexture::Repeat);
        texture.setWrapMode(QOpenGLTexture::DirectionT,QOpenGLTexture::Repeat);
        texture.setMinMagFilters(QOpenGLTexture::LinearMipMapLinear,QOpenGLTexture::Linear);
    }
};

class HFMesh : public QObject//, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    HFMesh(QOpenGLShaderProgram* shaderProgram, QOpenGLContext* OpenGLContext, QObject *parent = nullptr);
    void loadMeshData(QVector<Vertex> vertices, QVector<unsigned int> indices, QVector<Texture*> textures); // 加载Mesh所需数据
    void draw(); // 绘制函数
    void initMesh();
private:
    QOpenGLVertexArrayObject m_VAO;
    QOpenGLBuffer m_VBO;
    QOpenGLBuffer m_EBO;
    QOpenGLShaderProgram* m_shaderProgram;
    QOpenGLContext* m_OpenGLContext;

    QVector<Vertex>       m_vertices;                         // 顶点数据（顶点坐标、法线向量、纹理坐标）
    QVector<unsigned int> m_indices;                          // 索引数据
    QVector<Texture*>     m_textures;                         // 材质贴图数据
signals:

public slots:
};

#endif // HFMESH_H
