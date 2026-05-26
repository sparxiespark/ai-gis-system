#ifndef MODEL_DATAMODEL_H_A1B2
#define MODEL_DATAMODEL_H_A1B2

#include <QObject>
#include <vector>
#include <QString>

class DataModel : public QObject {
    Q_OBJECT
public:
    enum DataType { DataType_Unknown, DataType_CSV, DataType_Raster };

    explicit DataModel(QObject* pParent = nullptr);
    ~DataModel() override = default;

    void setValues(const std::vector<double>& _vValues, DataType _eType);
    const std::vector<double>& getValues() const;

    void setDimensions(int _nWidth, int _nHeight);
    int getWidth() const;
    int getHeight() const;

    DataType getDataType() const;

    void setAnalysisResult(const QString& _strResult);
    QString getAnalysisResult() const;

signals:
    // 【新增】数据更新信号，用于自动驱动视图渲染
    void signalDataUpdated(const std::vector<double>& _vData);

private:
    std::vector<double> m_vValues;
    DataType m_eDataType;
    int m_nWidth;
    int m_nHeight;
    QString m_strAnalysisResult;
};
#endif