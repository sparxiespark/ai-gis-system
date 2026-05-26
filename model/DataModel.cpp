#include "DataModel.h"

DataModel::DataModel(QObject* pParent)
    : QObject(pParent), m_eDataType(DataType_Unknown), m_nWidth(0), m_nHeight(0) {
}

void DataModel::setValues(const std::vector<double>& _vValues, DataType _eType) {
    m_vValues = _vValues;
    m_eDataType = _eType;

    // 【关键】数据被修改时，向外层发出通知
    emit signalDataUpdated(m_vValues);
}

const std::vector<double>& DataModel::getValues() const { return m_vValues; }

void DataModel::setDimensions(int _nWidth, int _nHeight) {
    m_nWidth = _nWidth;
    m_nHeight = _nHeight;
}

int DataModel::getWidth() const { return m_nWidth; }
int DataModel::getHeight() const { return m_nHeight; }

DataModel::DataType DataModel::getDataType() const { return m_eDataType; }

void DataModel::setAnalysisResult(const QString& _strResult) { m_strAnalysisResult = _strResult; }
QString DataModel::getAnalysisResult() const { return m_strAnalysisResult; }