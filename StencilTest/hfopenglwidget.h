#ifndef HFOPENGLWIDGET_H
#define HFOPENGLWIDGET_H

#include <QObject>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QTime>
#include "hfmodel.h"
#include "hfcamera.h"
class HFOpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
public:
    HFOpenGLWidget(QWidget* parent = nullptr);
    ~HFOpenGLWidget() override;
protected:
    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;
    void keyBoardProcess();

private:
    QOpenGLVertexArrayObject cubeVAO;
    QOpenGLBuffer cubeVBO;
    QOpenGLShaderProgram cubeShaderProgram;
    QOpenGLTexture cubeTexture;

    QOpenGLVertexArrayObject planeVAO;
    QOpenGLBuffer planeVBO;
    QOpenGLShaderProgram planeShaderProgram;
    QOpenGLTexture planeTexture;

    QOpenGLShaderProgram singleShaderProgram;

    bool keys[100] = {false}; // 保存键盘状态的数组
    HFCamera *m_camera; // 声明一个摄像头
    QPoint m_centerMousePoint;
    /* 帧率计算 */
    QTime m_time;
    int m_lastTime;
    int m_deltaTime;

    HFModel* model;
};

#endif // HFOPENGLWIDGET_H
