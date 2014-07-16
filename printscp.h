/*
 * Copyright (C) 2014 Irkutsk Diagnostic Center.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PRINTSCP_H
#define PRINTSCP_H

#include <QObject>
#include <QSettings>
#include <tesseract/baseapi.h>

#define DEFAULT_LISTEN_PORT 10005
#define DEFAULT_TIMEOUT     30

#define HAVE_CONFIG_H
#include <dcmtk/config/osconfig.h>   // make sure OS specific configuration is included first
#include <dcmtk/dcmpstat/dvpstyp.h>  // for enum types
#include <dcmtk/dcmnet/dimse.h>

class DicomImage;
class T_ASC_Association;

class PrintSCP : public QObject
{
    Q_OBJECT
public:
    explicit PrintSCP(QObject *parent = 0);
    ~PrintSCP();

    /** performs association negotiation for the Print SCP. Depending on the
     *  configuration file settings, Basic Grayscale Print and Presentation LUT
     *  are accepted with all uncompressed transfer syntaxes.
     *  If association negotiation is unsuccessful, an A-ASSOCIATE-RQ is sent
     *  and the association is dropped. If successful, an A-ASSOCIATE-AC is
     *  prepared but not (yet) sent.
     *  @param net DIMSE network over which to receive the association request
     *  @return result indicating whether association negotiation was successful,
     *    unsuccessful or whether termination of the server was requested.
     */
    DVPSAssociationNegotiationResult negotiateAssociation();

    /** confirms an association negotiated with negotiateAssociation() and handles
     *  all DIMSE communication for the Print SCP. Returns after the association
     *  has been released or aborted.
     */
    void handleClient();

    OFCondition initNetwork();

    bool associationWaiting();

  private:

    /// private undefined assignment operator
    PrintSCP& operator=(const PrintSCP&);

    /// private undefined copy constructor
    PrintSCP(const PrintSCP& copy);

    /** sends A-ASSOCIATION-RQ as the result of an unsuccesful association
     *  negotiation.
     *  @param isBadContext defines the reason for the A-ASSOCIATE-RQ.
     *    true indicates an incorrect application context, false sends back
     *    an unspecified reject with no reason and is used when termination
     *    of the server application has been initiated.
     *  @return ASC_NORMAL if successful, an error code otherwise.
     */
    OFCondition refuseAssociation(T_ASC_RejectParametersResult result, T_ASC_RejectParametersReason reson);

    /** destroys the association managed by this object.
     */
    void dropAssociations();

    /** handles any incoming N-GET-RQ message and sends back N-GET-RSP.
     *  @param rq request message
     *  @param presID presentation context over which the message was received
     *  @return DIMSE_NORMAL if successful, an error code otherwise
     */
    OFCondition handleNGet(T_DIMSE_Message& rq, T_ASC_PresentationContextID presID);

    /** handles any incoming N-SET-RQ message and sends back N-SET-RSP.
     *  @param rq request message
     *  @param presID presentation context over which the message was received
     *  @return DIMSE_NORMAL if successful, an error code otherwise
     */
    OFCondition handleNSet(T_DIMSE_Message& rq, T_ASC_PresentationContextID presID);

    /** handles any incoming N-ACTION-RQ message and sends back N-ACTION-RSP.
     *  @param rq request message
     *  @param presID presentation context over which the message was received
     *  @return DIMSE_NORMAL if successful, an error code otherwise
     */
    OFCondition handleNAction(T_DIMSE_Message& rq, T_ASC_PresentationContextID presID);

    /** handles any incoming N-CREATE-RQ message and sends back N-CREATE-RSP.
     *  @param rq request message
     *  @param presID presentation context over which the message was received
     *  @return DIMSE_NORMAL if successful, an error code otherwise
     */
    OFCondition handleNCreate(T_DIMSE_Message& rq, T_ASC_PresentationContextID presID);

    /** handles any incoming N-DELETE-RQ message and sends back N-DELETE-RSP.
     *  @param rq request message
     *  @param presID presentation context over which the message was received
     *  @return DIMSE_NORMAL if successful, an error code otherwise
     */
    OFCondition handleNDelete(T_DIMSE_Message& rq, T_ASC_PresentationContextID presID);

    /** handles any incoming C-ECHO-RQ message and sends back C-ECHO-RSP.
     *  @param rq request message
     *  @param presID presentation context over which the message was received
     *  @return DIMSE_NORMAL if successful, an error code otherwise
     */
    OFCondition handleCEcho(T_DIMSE_Message& rq, T_ASC_PresentationContextID presID);

    /** implements the N-GET operation for the Printer SOP Class.
     *  @param rq request message
     *  @param rsp response message, already initialized
     *  @param rspDataset response dataset passed back in this parameter (if any)
     */
    void printerNGet(T_DIMSE_Message& rq, T_DIMSE_Message& rsp, DcmDataset *& rspDataset);

    /** implements the N-SET operation for the Basic Film Session SOP Class.
     *  @param rq request message
     *  @param rqDataset request dataset, may be NULL
     *  @param rsp response message, already initialized
     *  @param rspDataset response dataset passed back in this parameter (if any)
     */
    void filmSessionNSet(T_DIMSE_Message& rq, DcmDataset *rqDataset, T_DIMSE_Message& rsp, DcmDataset *& rspDataset);

    /** implements the N-SET operation for the Basic Film Box SOP Class.
     *  @param rq request message
     *  @param rqDataset request dataset, may be NULL
     *  @param rsp response message, already initialized
     *  @param rspDataset response dataset passed back in this parameter (if any)
     */
    void filmBoxNSet(T_DIMSE_Message& rq, DcmDataset *rqDataset, T_DIMSE_Message& rsp, DcmDataset *& rspDataset);

    /** implements the N-SET operation for the Basic Grayscale Image Box SOP Class.
     *  @param rq request message
     *  @param rqDataset request dataset, may be NULL
     *  @param rsp response message, already initialized
     *  @param rspDataset response dataset passed back in this parameter (if any)
     */
    void imageBoxNSet(T_DIMSE_Message& rq, DcmDataset *rqDataset, T_DIMSE_Message& rsp, DcmDataset *& rspDataset);

    /** implements the N-ACTION operation for the Basic Film Session SOP Class.
     *  @param rq request message
     *  @param rsp response message, already initialized
     */
    void filmSessionNAction(T_DIMSE_Message& rq, T_DIMSE_Message& rsp);

    /** implements the N-ACTION operation for the Basic Film Box SOP Class.
     *  @param rq request message
     *  @param rsp response message, already initialized
     */
    void filmBoxNAction(T_DIMSE_Message& rq, T_DIMSE_Message& rsp);

    /** implements the N-CREATE operation for the Basic Film Session SOP Class.
     *  @param rqDataset request dataset, may be NULL
     *  @param rsp response message, already initialized
     *  @param rspDataset response dataset passed back in this parameter (if any)
     */
    void filmSessionNCreate(DcmDataset *rqDataset, T_DIMSE_Message& rsp, DcmDataset *& rspDataset);

    /** implements the N-CREATE operation for the Basic Film Box SOP Class.
     *  @param rqDataset request dataset, may be NULL
     *  @param rsp response message, already initialized
     *  @param rspDataset response dataset passed back in this parameter (if any)
     */
    void filmBoxNCreate(DcmDataset *rqDataset, T_DIMSE_Message& rsp, DcmDataset *& rspDataset);

    /** implements the N-CREATE operation for the Presentation LUT SOP Class.
     *  @param rqDataset request dataset, may be NULL
     *  @param rsp response message, already initialized
     *  @param rspDataset response dataset passed back in this parameter (if any)
     */
    void presentationLUTNCreate(DcmDataset *rqDataset, T_DIMSE_Message& rsp, DcmDataset *& rspDataset);

    /** implements the N-DELETE operation for the Basic Film Session SOP Class.
     *  @param rq request message
     *  @param rsp response message, already initialized
     */
    void filmSessionNDelete(T_DIMSE_Message& rq, T_DIMSE_Message& rsp);

    /** implements the N-DELETE operation for the Basic Film Box SOP Class.
     *  @param rq request message
     *  @param rsp response message, already initialized
     */
    void filmBoxNDelete(T_DIMSE_Message& rq, T_DIMSE_Message& rsp);

    /** implements the N-DELETE operation for the Presentation LUT SOP Class.
     *  @param rq request message
     *  @param rsp response message, already initialized
     */

    /** Add attributes from the printer settings.
     *  @param rqDataset request dataset, may not be NULL
     *  @param di image from dataset, may not be NULL
     *  @param settings to read attributes from
     */
    void insertTags(DcmDataset *rqDataset, QUrl *url, DicomImage *di, QSettings &settings);

    /* class data */

    // blocking mode for receive
    //
    T_DIMSE_BlockingMode blockMode;

    // timeout for receive
    //
    int timeout;

    // basic film session instance
    //
    QString filmSessionUID;
    QString studyInstanceUID;
    QString seriesInstanceUID;

    DcmDataset* filmBoxDataset;

    // Printer AETITLE. Must have a section in the settings file.
    //
    QString printer;

    // the DICOM network and listen port
    //
    T_ASC_Network *net;

    // the network association over which the print SCP is operating
    //
    T_ASC_Association *assoc;

    // the network association over which the real printer is operating
    //
    T_ASC_Association *upstream;

    // Should we eat the upstream failures?
    //
    bool ignoreUpstreamErrors;

    // OCR
    //
    tesseract::TessBaseAPI tess;
};

#endif // PRINTSCP_H
