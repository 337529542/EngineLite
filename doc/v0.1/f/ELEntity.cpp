#include "ELEntity.h"


void ELEntity::scale( const ELVector3& v )
{
	ELMatrix4x4 m2;
	m2.setScale(v);
	worldMatrix.mul(m2);

}

ELEntity::ELEntity()
{
	worldMatrix.resetMatrix();
}