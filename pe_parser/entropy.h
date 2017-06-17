#ifndef _entropy_h_
#define _entropy_h_

#include <math.h>
#ifdef QT_CORE_LIB
#include <QVector>
#else
#include <Windows.h>
#include <vector>
#endif

#include "macro.h"

const size_t cDefaultWindowSz = 0x400;

typedef double EntropyType;//����������� ����, ������� ����������� �������� �������� (����� ���� float - ��� �������)

class Entropy
{
private:
    EntropyType _value;
    size_t _size;//� ���� ���� ����������� ������ �����, ��� �������� �������������� ��������
    EntropyType _calc_entropy(unsigned char *pData, size_t DataSz);//�������� ����� �������
	size_t statistics[0x100]; /*
							  ������� ������ ��������. 
							  ����� ���� ��������� ������� 
							  ����� ����� ����� ������, 
							  ��� �������� �������������� ��������
							  */
public:
	/*
	�����������. �� ���� ����������� ��������� � ������ ����� ������, 
	��� �������� ����� ���������� ��������
	*/
    Entropy(unsigned char *pData, size_t DataSz);
	/*
	���������� ������������ ��������, 
	��������������� read-only ������ � �������� ��������
	*/
    const EntropyType value() { return _value; }
	__declspec(property(
                   get = value))
    const EntropyType Value;
	/*
	����������� �������� ������� �����
	*/
    const size_t frame_size() { return _size; }
    __declspec(property(
                   get = frame_size))
    const size_t Size;
	/*
	������ ��� ���������� � �������� ����� �� ����� - ����� 
	��� �������� ��������� �������� ����������� ���� ��� ��� �������� �� ������� ������.
	������� �� �������� ����� �� ������ ��� ������������������� ����, �� ����� ��� ��� �� �����.
	*/
    EntropyType append_byte(unsigned char val);
    EntropyType reduce_byte(unsigned char val);
};

/*
��������� ���� �������� � �������� �������.
���������� ��������: ���, ��� �� ���������� � �������� ��� X (��������),
����� �������������� EntropyX, � ���, ��� ���������� � �������� �-� (��������, ��� Y) - 
EntropyY. ��� ���� ����� �������� �����, ��������, 
EntropyX ����� ��� ������ �������� ��������� ������� float.
*/
typedef EntropyType EntropyX, EntropyY;
/*
���, ����������� �������������� �������� ��� �������� ����� X, ������, ��� ���� 
(� ����� ������ �������� 512 ����), ��������� �������� �������� ����� X.
5 ��������:
 - ��������
 - �����������
 - ��������� ����������
 - ��������� ���������
 - �������������
*/
typedef struct EntropyCharacteristics {
    EntropyY entropy_value;//���������� �������� ����, ��������� �������� �������� ����� X
    EntropyY entropy_derivative_value;//�������� ������ ����������� �������� � ������ �����
    EntropyY maximums_density;//���������� ���������� � ������ ����
    EntropyY minimums_density;//���������� ��������� � ������ ����.
	//������ ���� �������������� ����� �������� ����������� ����, �.�. �� ��������� ������� ��������.
	//����� ���������� ����������� �����������, ��� ����� ���� ���������� �����.
    EntropyY volatility;//������������� - ������� ����� ����� ������� � ����� ��������� 
	//��������� �������� � ������ ����, ���� �� ������������ � �� ��������������
} EntropyChars, EntropyPoint;

//������������, ������������ ��������� ������� ��������: ����, ����� � ���� �����.
enum EntropyDirection { edirMax = -1 /*�� ���������� ���������� ����*/, 
	edirNo = 0/*�����*/, 
	edirMin = 1/*�� ��������� ���������� ����*/ };

inline EntropyDirection e_sign(const EntropyType c_val) {//�-� ���������� ������ ����������� � ���� EntropyDirection
    return ((c_val < 0) ? edirMax : ((c_val > 0) ? edirMin : edirNo));
}
//������ ����������� ������� �� ���� ������ �����������
inline EntropyDirection e_dir(const EntropyY prev, const EntropyY cur) {
    if(prev * cur < 0)//���� ����������� ��������� 0 � ����� �����������, ��� �������� ����� �������������
        return e_sign(cur - prev);
    else
        return edirNo;
}

/*
���� � ������� ��� ������������ ��������
*/

//��������� ����������
typedef struct ExtremumPoint {
	EntropyX X;
	EntropyY Y;
	EntropyDirection eDir;
	ExtremumPoint(EntropyX x = 0., EntropyY y = 0.,
		EntropyDirection edir = edirNo) : X(x), Y(y),
		eDir(edir)
	{}
} *PExtremumPoint;

#ifdef QT_CORE_LIB
/*
�����������, �������������� �� ������������� Qt
*/
typedef QVector<EntropyPoint> EntropyDiagram;//��� ���������, ���������� �������� ��� ���� ��������
//������ ������������� - ���� �� ������������
inline EntropyY volatility(EntropyDiagram &diagram, const size_t cX, const size_t cWindowSz = cDefaultWindowSz) {
    const size_t cWinHalfSz = cWindowSz >> 1;
    EntropyY e_min = diagram[cX].entropy_value;
    EntropyY e_max = diagram[cX].entropy_value;
    for(int i = (int)(cX - cWinHalfSz); i < (int)(cX + cWinHalfSz); ++i) {
        if(i < 0) continue;
        if(i >= diagram.size())
            break;
        e_max = qMax(e_max, diagram[i].entropy_value);
        e_min = qMin(e_min, diagram[i].entropy_value);
    }
    return e_max - e_min;
}
//������ �����������
typedef QVector<ExtremumPoint> ExtremumPoints;

#else
//���� ���������� stl ������ Qt
typedef std::vector<EntropyPoint> EntropyDiagram;
typedef std::vector<ExtremumPoint> ExtremumPoints;
#endif

#endif
