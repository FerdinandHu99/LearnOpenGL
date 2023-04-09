#ifndef HFCAMERA_H
#define HFCAMERA_H

#include <QObject>
#include <QVector3D>
#include <QPoint>
#include <QDebug>
#include <QtMath>
#include <QMatrix4x4>
class HFCamera : public QObject
{
    Q_OBJECT
public:
    HFCamera(QObject *parent = nullptr, QVector3D position = QVector3D(0, 0, 6),
                      QVector3D orientation = QVector3D(0, 0, -1), QVector3D worldUp = QVector3D(0, 1, 0));
    ~HFCamera();

    enum KeyMovement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };
    Q_ENUM(KeyMovement)

    void       processMouseMovement(float xOffset, float yOffset);                          // 处理鼠标移动事件
    void       processMouseMidBtnMovement(float yOffset);                                   // 处理鼠标中键按下并移动鼠标
    void       processKeyboard(HFCamera::KeyMovement direction, float deltaTime);           // 处理键盘WSAD事件
    void       processMouseWheel(float wheelValue);                                         // 处理鼠标滚轮事件
    QMatrix4x4 getViewMatrix();                                                             // 获得观察矩阵
    float      Fov();                                                                       // 获得摄像机的视野角
    QVector3D  position();                                                                  // 获得摄像机的位置
    QVector3D  orientation();                                                               // 获得摄像机的拍摄方向
    QVector3D  up();                                                                        // 获得摄像机的真实y轴
    QVector3D  right();                                                                     // 获得摄像机的真实x轴
    float      Yaw();                                                                       // 获得摄像机的偏航角
    float      Pitch();                                                                     // 获得摄像机的俯仰角

private:
    QVector3D m_position;                    // 摄像机位置
    QVector3D m_orientation;                 // 摄像机拍摄方向
    QVector3D m_worldUp;                     // 摄像机的参考y轴
    QVector3D m_up;                          // 摄像机的真实y轴
    QVector3D m_right;                       // 摄像机的真实x轴
    float     m_Yaw;                         // 摄像机的偏航角
    float     m_Pitch;                       // 摄像机的俯仰角
    float     m_Fov;                         // 摄像机的视野范围角度
    float     m_keyControlSpeed;             // 按键控制速度
    float     m_mouseControlSpeed;           // 鼠标控制速度

private:
    float radians(float angle);              // 角度制换算成弧度制
    void  updateCameraVectors();             // 更新摄像机的向量
signals:
    void cameraUpdated();                    // 相机参数更新，发送信号

public slots:
};

#endif // HFCAMERA_H
