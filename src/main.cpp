/******************************************************
** See the accompanying README and LICENSE files
** Author(s): Jeremy Magland
** Created: 3/30/2016
*******************************************************/

#include "get_command_line_params.h"
#include "diskreadmda.h"
#include "mda.h"
#include <QDateTime>
#include <QDir>
#include <QString>
#include <QStringList>
#include "textfile.h"

void usage();
bool parse_index_string(const QString &str,long &i1,long &i2,long &i3);
bool parse_size_string(const QString &str,long &s1,long &s2,long &s3);
QString get_chunk_code(const QString &fname,const QString &datatype,long i1,long i2,long i3,long s1,long s2,long s3);

int main(int argc, char *argv[])
{
	CLParams CLP=get_command_line_params(argc,argv);
	QString arg0=CLP.unnamed_parameters.value(0);
	QString arg1=CLP.unnamed_parameters.value(1);

	if (arg0.isEmpty()) {
		usage();
		return 0;
	}

	if (arg0=="size") {
		if ((arg1.isEmpty())||(!arg1.endsWith(".mda"))) {
			printf("Problem with second argument\n");
			return -1;
		}
		if (!QFile::exists(arg1)) {
			printf("Input path does not exist: %s\n",arg1.toLatin1().data());
			return -1;
		}
		DiskReadMda X(arg1);
		printf("%ld,%ld,%ld,%ld,%ld,%ld\n",X.N1(),X.N2(),X.N3(),X.N4(),X.N5(),X.N6());
		return 0;
	}
	else if (arg0=="readChunk") {
		if ((arg1.isEmpty())||(!arg1.endsWith(".mda"))) {
			printf("Problem with second argument\n");
			return -1;
		}
		if (!QFile::exists(arg1)) {
			printf("Input path does not exist: %s\n",arg1.toLatin1().data());
			return -1;
		}
		QString outpath=CLP.named_parameters["outpath"].toString();;
		if (outpath.isEmpty()) {
			printf("outpath is empty\n");
			return -1;
		}
		QString index_string=CLP.named_parameters["index"].toString();
		long i1,i2,i3;
		if (!parse_index_string(index_string,i1,i2,i3)) {
			printf("Error parsing index\n");
			return -1;
		}
		QString size_string=CLP.named_parameters["size"].toString();
		long s1,s2,s3;
		if (!parse_size_string(size_string,s1,s2,s3)) {
			printf("Error parsing size\n");
			return -1;
		}
		QString datatype=CLP.named_parameters["datatype"].toString();
		if (datatype.isEmpty()) {
			printf("datatype is empty\n");
			return -1;
		}
		QString code=get_chunk_code(arg1,datatype,i1,i2,i3,s1,s2,s3);
		if (code.isEmpty()) {
			printf("Error computing chunk code.\n");
			return -1;
		}
		QString fname=outpath+"/"+code+".mda";
		if (!QFile::exists(fname)) {
			DiskReadMda X(arg1);
			Mda chunk;
			X.readChunk(chunk,i1,i2,i3,s1,s2,s3);

			if (datatype=="float32") {
				if (!chunk.write32(fname+".tmp")) {
					printf("Error writing file: %s.tmp\n",fname.toLatin1().data());
					return -1;
				}
			}
			else if (datatype=="float64") {
				if (!chunk.write64(fname+".tmp")) {
					printf("Error writing file: %s.tmp\n",fname.toLatin1().data());
					return -1;
				}
			}
			else {
				printf("Unsupported data type: %s\n",datatype.toLatin1().data());
				return -1;
			}
			DiskReadMda check(fname+".tmp");
            if (s2==0) s2=1;
            if (s3==0) s3=1;
            if ((check.N1()!=s1)||(check.N2()!=s2)||(check.N3()!=s3)) {
				printf("Unexpected dimensions of output file: %ld,%ld,%ld\n",check.N1(),check.N2(),check.N3());
				QFile::remove(fname+".tmp");
				return -1;
			}
			if (!QFile::rename(fname+".tmp",fname)) {
				printf("Error renaming file to %s\n",fname.toLatin1().data());
				return -1;
			}
		}
		else {
			DiskReadMda check(fname);
            if (s2==0) s2=1;
            if (s3==0) s3=1;
			if ((check.N1()!=s1)||(check.N2()!=s2)||(check.N3()!=s3)) {
				printf("Unexpected dimensions of existing output file: %ld,%ld,%ld\n",check.N1(),check.N2(),check.N3());
				QFile::remove(fname);
				return -1;
			}
		}

		printf("%s.mda\n",code.toLatin1().data());
		return 0;
	}
	else {
		printf("unexpected command\n");
		return -1;
	}
}

void usage() {
	printf("mdachunk size fname.mda\n");
	printf("mdachunk readChunk fname.mda --index=0,0,100 --size=500,300,300 --outpath=/output/path --datatype=float32|float64\n");
}

bool parse_index_string(const QString &str,long &i1,long &i2,long &i3) {
	QStringList vals=str.trimmed().split(",");
	if ((vals.count()<0)||(vals.count()>3)) {
		return false;
	}
	i1=vals.value(0,"0").toLong();
	i2=vals.value(1,"0").toLong();
	i3=vals.value(2,"0").toLong();
	return true;
}

bool parse_size_string(const QString &str,long &s1,long &s2,long &s3) {
	QStringList vals=str.trimmed().split(",");
	if ((vals.count()<0)||(vals.count()>3)) {
		return false;
	}
	s1=vals.value(0,"1").toLong();
	s2=vals.value(1,"1").toLong();
	s3=vals.value(2,"1").toLong();
    return (s1>0);
}

QString get_file_info(const QString &fname) {
    QString format="yyyy-MM-dd.hh.mm.ss.zzz";
    QString created=QFileInfo(fname).created().toString(format);
    QString modified=QFileInfo(fname).lastModified().toString(format);
    qint64 size=QFileInfo(fname).size();
    return QString("created=%1;modified=%2;size=%3").arg(created).arg(modified).arg(size);
}

bool is_out_of_date(const QString &sha1_fname,const QString &fname) {
    QString str=read_text_file(sha1_fname).split("\n").value(1); //the second line
    if (str.isEmpty()) return true;
    return (str!=get_file_info(fname));
}

QString get_sha1_code(const QString &fname) {
	QString sha1_fname=fname+".sha1";
	QString sha1;
    if ((!QFile::exists(sha1_fname))||(is_out_of_date(sha1_fname,fname))) {
		QString cmd=QString("sha1sum %1 > %2").arg(fname).arg(sha1_fname);
		if (system(cmd.toLatin1().data())!=0) {
			printf("Problem in system call: %s\n",cmd.toLatin1().data());
			return "";
		}
        QString content=read_text_file(sha1_fname);
        content=content.trimmed()+"\n"+get_file_info(fname);
        write_text_file(sha1_fname,content);
	}
	if (QFile::exists(sha1_fname)) {
		sha1=read_text_file(sha1_fname).trimmed();
		int ind=sha1.indexOf(" ");
		sha1=sha1.mid(0,ind);
	}
	if (sha1.count()!=40) {
		if (QFile::exists(sha1_fname)) QFile::remove(sha1_fname); //must be corrupted
		sha1="";
	}
	return sha1;
}

QString get_chunk_code(const QString &fname,const QString &datatype,long i1,long i2,long i3,long s1,long s2,long s3) {
	QString sha1=get_sha1_code(fname);
	return QString("%1.%2.%3.%4.%5.%6.%7.%8").arg(sha1).arg(datatype)
			.arg(i1).arg(i2).arg(i3)
			.arg(s1).arg(s2).arg(s3);
}
