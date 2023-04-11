#include "hfcamera.h"

HFCamera::HFCamera(QObject *parent, QVector3D position, QVector3D orientation, QVector3D worldUp) : QObject(parent)
{
    m_position          = position;
    m_orientation       = orientation;
    m_worldUp           = worldUp;
    m_Yaw               = 0.0f;
    m_Pitch             = 0.0f;
    m_Fov               = 45.0f;
    m_keyControlSpeed   = 0.005f;
    m_mouseControlSpeed = 0.1f;

    // 开始更新摄像机的向量
    updateCameraVectors();
}

HFCamera::~HFCamera()
{

}

void HFCamera::processMouseMovement(float xOffset, float yOffset)
{
    m_Yaw   -= (xOffset * m_mouseControlSpeed);
    m_Pitch -= (yOffset * m_mouseControlSpeed);

    if (m_Pitch >  89.0f) m_Pitch =  89.0f;
    if (m_Pitch < -89.0f) m_Pitch = -89.0f;

    updateCameraVectors();
}

void HFCamera::processMouseMidBtnMovement(float yOffset)
{
    float up = yOffset * m_mouseControlSpeed;
    m_position -= m_up * up;
}

void HFCamera::processKeyboard(HFCamera::KeyMovement direction, float deltaTime)
{
    float velocity = m_keyControlSpeed * deltaTime;
    if (direction == FORWARD)  m_position += m_orientation * velocity;
    if (direction == BACKWARD) m_position -= m_orientation * velocity;
    if (direction == LEFT)     m_position -= m_right * velocity;
    if (direction == RIGHT)    m_position += m_right * velocity;

    //m_position.setY(2.0);
    //qDebug() << m_position;
    emit cameraUpdated();
}

// 处理鼠标滚轮事件
void HFCamera::processMouseWheel(float wheelValue)
{
    if (m_Fov >= 1.0f && m_Fov <= 70.0f) m_Fov -= wheelValue/120.0f;
    if (m_Fov < 1.0f)                    m_Fov = 1.0f;
    if (m_Fov > 70.0f)                   m_Fov = 70.0f;
}

QMatrix4x4 HFCamera::getViewMatrix()
{
    QMatrix4x4 viewMatrix;
    viewMatrix.lookAt(m_position, m_position + m_orientation, m_worldUp);
    return  viewMatrix;
}

float HFCamera::Fov()
{
    return m_Fov;
}

QVector3D HFCamera::position()
{
    return m_position;
}

QVector3D HFCamera::orientation()
{
    return m_orientation;
}

QVector3D HFCamera::up()
{
    return m_up;
}

QVector3D HFCamera::right()
{
    return m_right;
}

float HFCamera::Yaw()
{
    return m_Yaw;
}

float HFCamera::Pitch()
{
    return m_Pitch;
}

// 将角度换算成弧度
float HFCamera::radians(float angle)
{
    return angle * M_PI / 180.0f;
}

void HFCamera::updateCameraVectors()
{
    QVector3D orientation;
    orientation.setX(cos(radians(m_Yaw)) * cos(radians(m_Pitch)));
    orientation.setY(sin(radians(m_Pitch)));
    orientation.setZ(-sin(radians(m_Yaw)) * cos(radians(m_Pitch)));


    m_orientation = orientation.normalized();
    m_right = QVector3D::crossProduct(m_orientation, m_worldUp).normalized();
    m_up = QVector3D::crossProduct(m_right, m_orientation).normalized();

    emit cameraUpdated();  // 相机的参数已经更新

    //QVector3D m_z = QVector3D::crossProduct(m_right, m_up).normalized();
    //qDebug() << m_orientation;
}
