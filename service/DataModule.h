#ifndef SERVICE_DATAMODULE_H_A1B2
#define SERVICE_DATAMODULE_H_A1B2

#include <QObject>

class QString;
class DataModel;

class DataModule : public QObject {
    Q_OBJECT
public:
    explicit DataModule(QObject* pParent = nullptr);
    ~DataModule() = default;

    // 签名修改：改为传入常驻模型的指针
    void readCsvData(const QString& _strFilePath, DataModel* pModel);
    void readRasterData(const QString& _strFilePath, DataModel* pModel);
};
#endif