CamServer is a lightweight camera based interative system written in C++.
The main platform is Windows, but linux and MacOSX are also supported.
It uses OpenCV and some libraries from OpenFrameworks.
Folder /Server contains the main application. 
Folder /Client contains some processing sketches.
Folder /Tutorial contains the source files of the tutorial.
OSC and TUIO protocol is employed to send reconginzed blob/face information to client softwares.

It now supports Kinect and PS3 Camera.

The tutorials are provided at [here](http://www.everbox.com/f/bwy7u4c3xDkmKpKMbtTWu7ojqR)
But it's a little bit outdated.

ʹ��˵��

CamServer.exe [-i camera/image/video] [-dos] [-log] [-minim] [-delay 5] [-client 192.168.1.122 ][-port 3333] [-face]

��һ��������������
camera		����ͷ�ı�ţ�Ĭ��Ϊ0������һ������ͷ��ͬʱ֧��ʹ��CLEye������ps3����ͷ
image		ͼƬ·��
video		��Ƶ�ļ���·��
�����ָ������򿪵�һ������ͷ
 
-client 192.168.1.122	ָ���ͻ��˵ĵ�ַ��Ĭ��Ϊlocalhost����������ַ
-port 3333	ָ���ͻ��˵Ķ˿ڣ�Ĭ��Ϊ3333
-face 1.5	����ʶ���ܣ�Ĭ�ϲ���������ָ����������������δʵ�֣�
-finger		��ָʶ���ܣ�Ĭ�ϲ�����
-hand		��ָʶ���ܣ�Ĭ�ϲ�������δʵ��
-minim		����ʱΪ������С��
-delay	5	�ӳ�5���������
-log 		���ļ��м�¼����״����Ĭ�ϲ�����