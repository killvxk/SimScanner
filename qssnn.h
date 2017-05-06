#ifndef QSSNN_H
#define QSSNN_H

#include <QObject>
#include <opennn.h>

#include "macro.h"

class SimScanNN : public QObject
{
    Q_OBJECT
public:
    typedef unsigned int PROFILE_ID;
    enum class ssNNType {
        nntUnknown = -1,
        nntFullFileImage = 0,
        nntAnySection,
        nntCodeSection,
        nntResource,
        nntData,
        nntOverlay,

        nntCount
    };

    unsigned int get_inputs_count() {
        return 0;
    }
    __declspec(property(
        get = get_inputs_count))
    unsigned int inputsCount;

    explicit SimScanNN(tinyxml2::XMLDocument &xml,
              QObject *parent = 0);
    explicit SimScanNN(const char *xml_str,
              QObject *parent = 0);
    explicit SimScanNN(QObject *parent = 0);

    QString toString();
    static SimScanNN *fromString(QString &str);

    static QString DefaultEmpty();
signals:
    void signalSimilarity(double similarity);
public slots:
    //
protected:
    OpenNN::NeuralNetwork nn;
    SimScanNN::ssNNType nn_type;
};

#endif // QSSNN_H
