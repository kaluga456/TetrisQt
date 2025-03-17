#include <QSettings>
#include <QCoreApplication>
#include <QApplication>
#include <QScreen>
#include "about.h"
#include "options.h"

COptions Options;

bool CBestScore::add(int result)
{
    return add(result, QDate().currentDate());
}
bool CBestScore::add(int result, const QDate &date)
{
    if(result <= 0) return false;
    if(Results.size() < MAX_RESULTS)
    {
        Results[result] = date;
        return true;
    }

    std::map<int, QDate>::reverse_iterator i = Results.rbegin();
    if(i->first < result)
    {
        Results[result] = date;
        return true;
    }

    return false;
}

COptions::COptions()
{
    QCoreApplication::setOrganizationDomain(APP_URL);
    QCoreApplication::setOrganizationName(APP_COMPANY);
    QCoreApplication::setApplicationName(APP_NAME);
}
void COptions::Load()
{
    QSettings settings;

    LayoutLeft = settings.value("LayoutLeft", 200).toInt();
    LayoutTop = settings.value("LayoutTop", 300).toInt();

    //best results
    const int size = settings.beginReadArray("scores");
    for (int i = 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        int result = settings.value("result").toInt();
        QDate date = settings.value("date").toDate();
        BestResults.add(result, date);
    }
    settings.endArray();
}
void COptions::Save()
{
    QSettings settings;

    settings.setValue("LayoutLeft", LayoutLeft);
    settings.setValue("LayoutTop", LayoutTop);

    //best results
    settings.beginWriteArray("scores");
    int index = 0;
    for(CBestScore::const_iterator i = BestResults.beign(); i != BestResults.end(); ++i, ++index)
    {
        settings.setArrayIndex(index);
        settings.setValue("result", i->first);
        settings.setValue("date", i->second);
    }
    settings.endArray();
}


