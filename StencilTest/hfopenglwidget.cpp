#include "hfopenglwidget.h"
#include <QDebug>
#include <QMatrix4x4>
HFOpenGLWidget::HFOpenGLWidget(QWidget *parent) : QOpenGLWidget(parent), m_camera(new HFCamera),
    cubeVBO(QOpenGLBuffer::VertexBuffer), planeVBO(QOpenGLBuffer::VertexBuffer),
    cubeTexture(QOpenGLTexture::Target2D), planeTexture(QOpenGLTexture::Target2D)
{
    //m_texture = new Texture;
    //m_texture.create();
    this->setFocusPolicy(Qt::StrongFocus);                       // 获得键盘焦点
    this->setMouseTracking(true);                                // 启用鼠标跟踪
    this->setCursor(Qt::BlankCursor);                            // 取消光标显示
    /* 时间开始 */
    m_lastTime = 0;
    m_time.start();


}

void HFOpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    m_centerMousePoint = QPoint(width()/2, height()/2);

    glEnable(GL_DEPTH_TEST); // 开启深度测试
    glDepthFunc(GL_LESS); // 小于深度缓冲则通过测试

    glEnable(GL_STENCIL_TEST); // 开启模板测试
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF); // 模板缓冲区的值不等于参考值时通过测试
    /* 模板测试失败时不修改模板缓冲区中的值
     * 模板测试通过、但深度测试失败时不修改模板缓冲区的值
     * 模板测试和深度测试都通过时将模板缓冲区中的值设置为参考值
     */
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    float cubeVertices[] = {
        // positions          // texture Coords
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    float planeVertices[] = {
        // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
        5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

        5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
        5.0f, -0.5f, -5.0f,  2.0f, 2.0f
    };

    /* 创建边框颜色shader程序并编译 */
    singleShaderProgram.create();
    singleShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/vertexshader.vert");
    singleShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/singlefragshader.frag");
    singleShaderProgram.link();

    cubeVAO.create();
    cubeVBO.create();
    cubeVAO.bind();
    cubeVBO.bind();
    cubeVBO.allocate(cubeVertices, sizeof (cubeVertices));
    cubeVBO.setUsagePattern(QOpenGLBuffer::StaticDraw);

    /* 设置cube纹理图片 */
    cubeTexture.create();
    cubeTexture.bind();
    cubeTexture.setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
    cubeTexture.setWrapMode(QOpenGLTexture::Repeat);
    QImage cubeImage(":/Textures/marble.jpg");
    cubeTexture.setData(cubeImage);

    /* 创建shader程序并编译 */
    cubeShaderProgram.create();
    cubeShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/vertexshader.vert");
    cubeShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/fragshader.frag");
    cubeShaderProgram.link();

    cubeShaderProgram.bind();
    /* 设置顶点坐标 */
    cubeShaderProgram.setAttributeBuffer(0, GL_FLOAT, 0 * sizeof (GL_FLOAT), 3, 5 * sizeof(GL_FLOAT));
    cubeShaderProgram.enableAttributeArray(0);
    /* 设置纹理坐标 */
    cubeShaderProgram.setAttributeBuffer(1, GL_FLOAT, 3 * sizeof (GL_FLOAT), 2, 5 * sizeof (GL_FLOAT));
    cubeShaderProgram.enableAttributeArray(1);
    cubeShaderProgram.release();
    cubeVAO.release();



    planeVAO.create();
    planeVBO.create();
    planeVAO.bind();
    planeVBO.bind();
    planeVBO.allocate(planeVertices, sizeof (planeVertices));
    planeVBO.setUsagePattern(QOpenGLBuffer::StaticDraw);

    /* 设置plane纹理图片 */
    planeTexture.create();
    planeTexture.bind();
    planeTexture.setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
    planeTexture.setWrapMode(QOpenGLTexture::Repeat);
    QImage planeImage(":/Textures/metal.png");
    planeTexture.setData(planeImage);

    planeShaderProgram.create();
    planeShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/vertexshader.vert");
    planeShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/fragshader.frag");
    planeShaderProgram.link();

    planeShaderProgram.bind();
    /* 设置顶点坐标 */
    planeShaderProgram.setAttributeBuffer(0, GL_FLOAT, 0 * sizeof (GL_FLOAT), 3 , 5 * sizeof(GL_FLOAT));
    planeShaderProgram.enableAttributeArray(0);
    /* 设置纹理坐标 */
    planeShaderProgram.setAttributeBuffer(1, GL_FLOAT, 3 * sizeof (GL_FLOAT), 2, 5 * sizeof (GL_FLOAT));
    planeShaderProgram.enableAttributeArray(1);

    planeShaderProgram.release();
    planeVAO.release();



}
void HFOpenGLWidget::paintGL()
{
    /* 计算帧率 */
    int currentTime = m_time.elapsed();
    m_deltaTime = currentTime - m_lastTime;
    int FPS = (1.0 / m_deltaTime) * 1000;
    m_lastTime = currentTime;

    /* 接受接盘更新 */
    keyBoardProcess();

    /* 清空并更新背景颜色,清除颜色缓冲区、深度缓冲区 */
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_TEST | GL_STENCIL_BUFFER_BIT);

    /* 设置MVP变换矩阵 */
    QMatrix4x4 cube_modelMat, plane_modelMat, viewMat, projectionMat;
    projectionMat.perspective(m_camera->Fov(), width()/height(), 0.1f, 100.0f);

    singleShaderProgram.bind();
    singleShaderProgram.setUniformValue("view", m_camera->getViewMatrix());
    singleShaderProgram.setUniformValue("projection", projectionMat);
    cubeShaderProgram.bind();
    cubeShaderProgram.setUniformValue("view", m_camera->getViewMatrix());
    cubeShaderProgram.setUniformValue("projection", projectionMat);
    planeShaderProgram.bind();
    planeShaderProgram.setUniformValue("view", m_camera->getViewMatrix());
    planeShaderProgram.setUniformValue("projection", projectionMat);

    /* 正常绘制地板, 不写模板缓冲 */
    glStencilMask(0x00); // 禁止模板缓冲区写入
    planeVAO.bind();
    planeShaderProgram.bind();
    planeTexture.bind(0);
    planeShaderProgram.setUniformValue("texture1", 0);
    planeShaderProgram.setUniformValue("model", plane_modelMat);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    planeVAO.release();

    /* 正常绘制箱子，写入模板缓冲 */
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
    cubeVAO.bind();
    cubeShaderProgram.bind();
    cubeTexture.bind(0);
    cubeShaderProgram.setUniformValue("texture1", 0);
    cube_modelMat.translate(QVector3D(-1, 0, -1));
    cubeShaderProgram.setUniformValue("model", cube_modelMat);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    cubeShaderProgram.release();
    cubeVAO.release();

    /* 绘制放大的箱子 */
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST); // 关闭深度测试，使得边框可以在在地板前面显示
    cubeVAO.bind();
    singleShaderProgram.bind();
    cubeTexture.bind(0);
    singleShaderProgram.setUniformValue("texture1", 0);
    cube_modelMat.scale(QVector3D(1.1f, 1.1f, 1.1f));
    singleShaderProgram.setUniformValue("model", cube_modelMat);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    singleShaderProgram.release();
    cubeVAO.release();

    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    glEnable(GL_DEPTH_TEST);

    update();
}

void HFOpenGLWidget::resizeGL(int w, int h)
{
    //glViewport(0, 0, w, h);
}

HFOpenGLWidget::~HFOpenGLWidget()
{
    makeCurrent(); // 获取当前上下文属性
    //QOpenGLWidget::~QOpenGLWidget();
    //::~QOpenGLFunctions_3_3_Core();
}

// 接收键盘按下事件
void HFOpenGLWidget::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    if (key == Qt::Key_Escape) {
        //emit EscBtnPressed();  // 当Esc按下时，发射信号给主窗口
    }
    if (key >= 0 && key < 1024) {
        if (event->type() == QEvent::KeyPress) {
            keys[key] = true;
        }
    }
}

// 接收键盘松开事件
void HFOpenGLWidget::keyReleaseEvent(QKeyEvent *event)
{
    int key = event->key();
    if (key >= 0 && key < 1024) {
        if (event->type() == QEvent::KeyRelease) {
            keys[key] = false;
        }
    }
}

// 键盘事件处理函数（实现多个按键同时按下功能）
void HFOpenGLWidget::keyBoardProcess()
{
    if (keys[Qt::Key_W] == true) m_camera->processKeyboard(HFCamera::FORWARD, m_deltaTime);
    if (keys[Qt::Key_S] == true) m_camera->processKeyboard(HFCamera::BACKWARD, m_deltaTime);
    if (keys[Qt::Key_A] == true) m_camera->processKeyboard(HFCamera::LEFT, m_deltaTime);
    if (keys[Qt::Key_D] == true) m_camera->processKeyboard(HFCamera::RIGHT, m_deltaTime);
}

// 接收鼠标移动事件
void HFOpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint currentMousePoint = event->pos();
    QPoint deltaMousePoint = currentMousePoint - m_centerMousePoint;
    if (event->buttons() == Qt::MidButton) {
        m_camera->processMouseMidBtnMovement(deltaMousePoint.y());
        QPoint center = this->mapToGlobal(m_centerMousePoint);
        QCursor::setPos(center);
    } else {
        m_camera->processMouseMovement(deltaMousePoint.x(), deltaMousePoint.y());
        QPoint center = this->mapToGlobal(m_centerMousePoint);
        QCursor::setPos(center);
    }
}

// 鼠标滚轮事件
void HFOpenGLWidget::wheelEvent(QWheelEvent *event)
{
    m_camera->processMouseWheel(event->angleDelta().y());
}
