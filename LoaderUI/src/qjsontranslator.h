#ifndef QJSONTRANSLATOR_H
#define QJSONTRANSLATOR_H

#include <QTranslator>
#include <QLocale>
#include <QVariant>

extern int loadLanguage(QLocale *locale = NULL);

class QJsonTranslator : public QTranslator
{
	Q_OBJECT

public:
	static QJsonTranslator* instance	(void); 
	static int				loadLanguage(const QLocale *lang = 0);

	~QJsonTranslator();

	virtual QString	translate(const char *context, const char *sourceText, 
		                      const char *disambiguation = 0) const;
	        QString translate(const char *context, const char *sourceText, 
				              const char *disambiguation, int n) const;

	virtual bool isEmpty() const;

	bool load(const QLocale &locale, 
			  const QString &filename,
			  const QString &prefix = QString(),
			  const QString &directory = QString(),
			  const QString &suffix = QString());
private:
	QJsonTranslator(QObject *parent = NULL);
	void		checkSynonymous(QString &check) const;

	bool		do_load(const QString &realname);

	bool		pLoaded;
	QVariant	pLanguageStrip;
	QLocale     pCurrentLocale;
};

#endif // QJSONTRANSLATOR_H
