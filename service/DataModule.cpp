#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif

#include "DataModule.h"
#include "../model/DataModel.h"
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <vector>
#include <cstdlib>
#include <gdal_priv.h>
#include <cpl_conv.h>

DataModule::DataModule(QObject* pParent) : QObject(pParent) {
    GDALAllRegister();
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
}

void DataModule::readCsvData(const QString& _strFilePath, DataModel* pModel) {
    if (!pModel) return;
    std::vector<double> _vMockData;

    QFile _fiInput(_strFilePath);
    if (_fiInput.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream _inStream(&_fiInput);
        while (!_inStream.atEnd()) {
            QString _strLine = _inStream.readLine();
            bool _bOk;
            double _dVal = _strLine.split(',').last().toDouble(&_bOk);
            if (_bOk) _vMockData.push_back(_dVal);
        }
        _fiInput.close();
    }
    else {
        for (int nIdx = 0; nIdx < 50; ++nIdx) {
            _vMockData.push_back((std::rand() % 100) / 10.0);
        }
    }
    // 注入数据，内部会触发 signalDataUpdated
    pModel->setValues(_vMockData, DataModel::DataType_CSV);
}

void DataModule::readRasterData(const QString& _strFilePath, DataModel* pModel) {
    if (!pModel) return;
    std::vector<double> _vRasterData;
    int _nRasterWidth = 0;
    int _nRasterHeight = 0;

    QFileInfo _fiInfo(_strFilePath);
    QString _strSuffix = _fiInfo.suffix().toLower();

    if (_strSuffix == "tif" || _strSuffix == "tiff") {
        GDALDataset* poDataset = (GDALDataset*)GDALOpen(_strFilePath.toLocal8Bit().data(), GA_ReadOnly);
        if (poDataset != nullptr) {
            GDALRasterBand* poBand = poDataset->GetRasterBand(1);
            if (poBand != nullptr) {
                _nRasterWidth = poBand->GetXSize();
                _nRasterHeight = poBand->GetYSize();
                int _nTotalPixels = _nRasterWidth * _nRasterHeight;

                _vRasterData.resize(_nTotalPixels);
                CPLErr _errStatus = poBand->RasterIO(GF_Read, 0, 0, _nRasterWidth, _nRasterHeight,
                    _vRasterData.data(), _nRasterWidth, _nRasterHeight, GDT_Float64, 0, 0);

                if (_errStatus == CE_None) {
                    pModel->setAnalysisResult(QString("GDAL 解析完毕: %1x%2 像素").arg(_nRasterWidth).arg(_nRasterHeight));
                }
                else {
                    pModel->setAnalysisResult("[失败] GDAL 读取像元错误");
                }
            }
            GDALClose((GDALDatasetH)poDataset);
        }
        else {
            pModel->setAnalysisResult("[失败] GDAL 无法打开此 TIFF");
        }
    }
    else {
        QFile _fiInput(_strFilePath);
        if (_fiInput.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream _inStream(&_fiInput);
            while (!_inStream.atEnd()) {
                QStringList _strParts = _inStream.readLine().split(' ', Qt::SkipEmptyParts);
                for (const QString& _strVal : _strParts) { _vRasterData.push_back(_strVal.toDouble()); }
            }
            _fiInput.close();
            _nRasterWidth = _vRasterData.size();
            _nRasterHeight = 1;
        }
        else {
            _nRasterWidth = 10; _nRasterHeight = 10;
            for (int nIdx = 0; nIdx < 100; ++nIdx) { _vRasterData.push_back(100.0 + (std::rand() % 50)); }
        }
    }

    pModel->setDimensions(_nRasterWidth, _nRasterHeight);
    pModel->setValues(_vRasterData, DataModel::DataType_Raster);
}