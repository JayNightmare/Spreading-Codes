LunaNet Signal-In-Space Recommended
Standard - Augmented Forward Signal
(LSIS - AFS)
VOLUME A

Version 1

Noted as Applicable Document 1 [AD1 Vol-A] in LNIS V5

LSIS V1.0

29 January 2025

1

PREFACE

LunaNet Interoperability Specification (LNIS)

The set of documents that comprise the LunaNet Interoperability Specification (LNIS) defines a framework
of  mutually  agreed-upon  standards,  protocols,  and  interface  specifications  that  enable  interoperability.
LunaNet is envisioned as a network of cooperating networks (network of networks, akin to the terrestrial
Internet) upon which providers can deliver communications, position, navigation, and timing, and other
services for users in transit to, around, and on the Moon.

This  document  is  controlled  by  the  NASA  Headquarters  (HQ)  Space  Communications  and  Navigation
(SCaN) Control Board (SCB). This document will be updated by Documentation Change Notice (DCN) or
complete  revision.  Proposed  changes  to  this  document  must  be  submitted  to  SCaN  Configuration
Management Office (CMO) along with supportive material justifying the proposed change.

Comments or questions concerning this document, and proposed changes shall be addressed to:

SCaN Control Board Secretariat

Amanda.R.Allen@nasa.gov

+1-240-565-3048

NASA Headquarters

300 Hidden Figures Way

Washington, DC 20546

LSIS V1.0

29 January 2025

2

Change Information Page

Document
Number

Status/Issue

Effective Date

CR Number

Description of
Change

LSIS V1.0

Baseline

January 29, 2025  ESC-CCR-0690

Initial Release

LSIS V1.0

29 January 2025

3

Table of Contents

Preface .......................................................................................................................................................... 2

1.  Introduction ............................................................................................................................................. 8

1.1.

1.2.

Scope ........................................................................................................................................... 9

Definitions and Annexes ............................................................................................................ 10

1.2.1.
1.2.2.

Definitions ....................................................................................................................................... 10
Annexes ........................................................................................................................................... 10
2.  Augmented Forward Signal Specifications............................................................................................. 11

2.1.

Interface Definition.................................................................................................................... 11

2.1.1.

AFS Signal and Data Structure ........................................................................................................ 11
AFS Signal-in-Space Error Definition .......................................................................................... 12

2.2.

2.2.1.
2.2.2.
2.2.3.

Signal-in-Space Error Concept ......................................................................................................... 12
Signal-in-Space Error Mathematical Definition ............................................................................... 12
Signal-in-Space Error Specification .................................................................................................. 13
AFS Signal Specifications ........................................................................................................... 14

2.3.

2.3.1.
2.3.2.
2.3.3.
2.3.4.
2.3.5.

Composite Signal ............................................................................................................................. 14
Modulation ...................................................................................................................................... 16
Logic Levels ..................................................................................................................................... 18
Transmitted Signals Coherency ....................................................................................................... 18
Spreading codes characteristics ...................................................................................................... 19
AFS Navigation message format specification .......................................................................... 24

2.4.

2.4.1.
2.4.2.
2.4.3.

General navigation message structure ........................................................................................... 24
Subframe 1 message specification .................................................................................................. 25
Frame structure ............................................................................................................................... 27
AFS Messages and data content ................................................................................................ 37

2.5.

2.5.1.
2.5.2.
2.5.3.
2.5.4.
2.5.5.
2.5.6.
2.5.7.
2.5.8.
2.5.9.
2.5.10.
2.5.11.
2.5.12.
2.5.13.
2.5.14.
2.5.15.
2.5.16.
2.5.17.
2.5.18.

LunaNet Network Access Information (MSG-G1) ............................................................................ 37
Health and Safety (MSG-G2) ........................................................................................................... 37
Clock and Ephemeris data (CED, MSG-G4) ...................................................................................... 37
MOrbit Almanac (MSG-G5) ............................................................................................................. 38
Time of transmission (MSG-G8) ...................................................................................................... 38
Maneuver (MSG-G10) ..................................................................................................................... 40
SAttitude State/ Ephemeris (MSG-G11) .......................................................................................... 40
Conjunction (MSG-G14) .................................................................................................................. 40
Maplet (MSG-G15) .......................................................................................................................... 40
Ancillary info (MSG-G17) ................................................................................................................. 40
LunaSAR Automatic Acknowledge (MSG-S19) ................................................................................ 40
LunaSAR Return Message (MSG-S20) ............................................................................................. 40
Acknowledge of non-SAR MSG (MSG-G22) ..................................................................................... 40
GNSS Augmentation (MSG-G23) ..................................................................................................... 41
Detection Alert (MSG-G24) ............................................................................................................. 41
Science (MSG-G25) .......................................................................................................................... 41
UIS Response (MSG-G27) ................................................................................................................ 41
User Schedule Notice (MSG-G28) .................................................................................................... 41

LSIS V1.0

29 January 2025

4

2.5.19.
2.5.20.
2.5.21.
2.5.22.

FF Commands (MSG-G29) ............................................................................................................... 41
Time Conversions (MSG-G30) .......................................................................................................... 41
Augmentation Differential Corrections (MSG-G31) ........................................................................ 41
Coordinate Frame Conversions (MSG-G32) ..................................................................................... 41
Appendix A – Acronyms .............................................................................................................................. 42

Appendix B – Signal-In-Space Error Contributors ....................................................................................... 44

Appendix C – Data Channel (AFS-I) Primary Codes ..................................................................................... 47

Appendix D – Pilot Channel (AFS-Q) Primary Codes ................................................................................... 51

Appendix E – Pilot Channel (AFS-Q) Tertiary Codes.................................................................................... 57

Appendix F – Code Assignments per LNSP.................................................................................................. 63

Appendix G – List of Algorithms .................................................................................................................. 69

Appendix H – Cyclic Redundancy Check Definition .................................................................................... 70

Appendix I – Reference AFS LInk Budget .................................................................................................... 71

APPENDIX ZZ– Table of TBXs ....................................................................................................................... 72

List of Tables

Table 1 - LNSP SISE Position ..................................................................................................................... 13

Table 2 - LNSP Velocity .............................................................................................................................. 13

Table 3 - Power Ratio between AFS-I and AFS-Q signal components ....................................................... 15

Table 4 - Received Minimum and Maximum Power ................................................................................... 15

Table 5 - Received Maximum Power .......................................................................................................... 16

Table 6 - Description of Signal Parameters ............................................................................................... 18

Table 7 - AFS Chip Rates and Symbol Rates .............................................................................................. 18

Table 8 - Logical to Signal Level Assignment ............................................................................................. 18

Table 9 - Code Lengths ............................................................................................................................... 19

Table 10 - Secondary Code Definitions ...................................................................................................... 22

Table 11 – Interim Test and Integration Code Assignments ....................................................................... 23

Table 12 - Synchronization Pattern ............................................................................................................ 25

Table 13: Subframe 1 Fields Specification ................................................................................................. 27

Table 14: Bit Allocation for Subframe 1 ..................................................................................................... 27

Table 15: Frame Structure Identification ................................................................................................... 27

Table 16 - Frame ID=0 Bits and Symbols Allocation to Subframes ........................................................... 31

Table 17 - Interleaving Parameters ............................................................................................................ 31

Table 18 - Bit Allocation for Subframe 2 .................................................................................................... 32

LSIS V1.0

29 January 2025

5

Table 19 – Bit Allocation for Subframe 3 ................................................................................................... 32

Table 20 – Bit Allocation for Subframe 4 ................................................................................................... 33

Table 21 - LNIS Message Allocation to AFS Subframes............................................................................. 35

Table 22 - Time of Transmission Fields Binary Representation ................................................................. 40

Table A- 1 Acronyms ................................................................................................................................... 42

- Table C- 1: Gold code initializations for PRNs 1-42 ............................................................................... 48

- Table C- 2:  Gold code initializations for PRNs 43-84 ............................................................................ 48

- Table C- 3: Gold code initializations for PRNs 85-126 ........................................................................... 49

- Table C- 4: Gold code initializations for PRNs 127-168 ......................................................................... 49

- Table C- 5: Gold code initializations for PRNs 169-210 ......................................................................... 50

Table D- 1- AFS-Q Primary Code 10233 Legendre Sequence in HEX ...................................................... 52

Table D- 2: Weil code index k and insertion index p for PRN 1-42 ........................................................... 54

Table D- 3: Weil code index k and insertion index p for PRN 43-84 ......................................................... 54

Table D- 4:- Weil code index k and insertion index p for PRN 85-126 ...................................................... 55

Table D- 5: Weil code index k and insertion index p for PRN 127-168 ..................................................... 55

Table D- 6: Weil code index k and insertion index p for PRN 169-210 ..................................................... 56

Table E- 1: AFS-Q Tertiary Code Length-1499 Legendre Sequence in HEX ............................................ 57

Table E- 2: 1500 Chip tertiary Weil code index k for PRN 1-42 ................................................................ 58

Table E- 3: 1500 Chip tertiary Weil code index k for PRN 43-84 .............................................................. 59

Table E- 4:- 1500 Chip tertiary Weil code index k for PRN 85-126 ........................................................... 60

Table E- 5- 1500 Chip tertiary Weil code index k for PRN 127-168 .......................................................... 61

Table E- 6: - 1500 Chip tertiary Weil code index k for PRN 169-210 ........................................................ 62

Table F- 1: - Code Assignments per LNSP node Identifier {LSIS-TBD-2001} ........................................... 63

Table ZZ- 1: Table of TBXs ........................................................................................................................ 72

LSIS V1.0

29 January 2025

6

List of Figures

Figure 1: PNT Services Provided by Multiple LNSPs .................................................................................. 8

Figure 2: Modulation Scheme for AFS Signal ............................................................................................ 17

Figure 3: Data and Pilot Channel Code and Data Synchronization .......................................................... 20

Figure 4: Tiered Codes Generation with Secondary and Tertiary Codes .................................................. 21

Figure 5: Navigation Message Generic Structure ...................................................................................... 24

Figure 6: Bit and Byte Ordering ................................................................................................................. 25

Figure 7: Subframe 1 BCH Encoding ......................................................................................................... 26

Figure 8: Example of BCH(51,8) encoder for subframe-1 (SB1) ............................................................... 26

Figure 9: Frame Structure of Frame ID Equal to Zero .............................................................................. 28

Figure 10: LDPC Submatrices ................................................................................................................... 30

Figure 11:  Time of Transmission Concept ................................................................................................ 39

Figure B- 1: Representative illustration of ephemeris errors ..................................................................... 44

Figure B- 2: Representative illustration of antenna offset errors ............................................................... 45

Figure B- 3: Representative illustration of main LNSP node error contributors ....................................... 46

Figure C- 1: Data Channel (AFS-I) Gold code generator. ........................................................................ 47

Figure E- 1: Tertiary Weil code generation process. ................................................................................. 58

LSIS V1.0

29 January 2025

7

1. INTRODUCTION

This  document  forms  part  of  the  LunaNet  Interoperability  Specification  (LNIS)  and  describes the
characteristics of the signals and messages to support, at a minimum, the Position Navigation and Time
(PNT) aspects of the Lunar Augmented Navigation Service (LANS) defined in the LNIS. This document
covers specifications to ensure interoperability among LunaNet Service Providers (LNSPs) supporting the
LANS. As defined in LNIS, LANS is provided by multiple provider nodes to multiple users at the same
time, using a concept similar to Global Navigation Satellite Systems (GNSS). This service is to be provided
in the 2483.5-2500 MHz band via the Augmented Forward Signal (AFS).

Figure 1 outlines the LNIS Position, Navigation, and Timing (PNT) services. Note that the different colors
in the top left box represent different LNSPs contributing to LANS through one or multiple nodes. LANS
is shown on the left, and PNT from Point-to-Point (P2P) communication signals is shown towards the right.
This document (Applicable Document 1 - Volume A) provides specifications for LANS and AFS (including
message definitions which are disseminated through AFS) (blue color) to ensure interoperability between
different nodes from multiple LNSPs (blue, orange, green and purple colors), while Applicable Document
1 - Volume B provides recommended standards for P2P signals (including those involved in implementing
PNT services).

Figure 1: PNT Services Provided by Multiple LNSPs

This document is a recommended standard. The adoption of the standard by a service provider is voluntary,
however, a service provider that claims to be LunaNet LANS compliant must conform to this recommended
standard in all its parts.

LunaNet may encompass systems of different nature, including different orbits, different Earth segments,
etc.  Thus,  some aspects  specific to  LANS might  be  implemented  differently  by  different  LNSPs.  This
standard  aims  at  balancing  the  establishment  of LANS interoperability,  while  providing  flexibility to

LSIS V1.0

29 January 2025

8

LNSPs in certain areas of implementation. For this reason, this document implements two categories of
specifications, broadly defined as follows:

1)  Precise  specifications  that  are  provided  to  ensure  the  definition  of  key  interoperability  items,

without ambiguity and with no flexibility to LNSP implementation.

2)  Functional specifications that provide guidance to guarantee interoperability yet provide flexibility

to LNSPs to define specific implementations.

Functional specifications in this document, per number 2 above, are indicated by incorporating “FLEX”
within the applicable specification identifiers and/or via the incorporation of the following text: “Note: This
definition  is  a  functional  specification  intended  to  provide  the  LNSP  with  implementation  flexibility”.
Examples  of  LNSP  implementation  flexibility  include  but  are  not  limited  to  the  message  content
dissemination cadence, the update rate of the message, etc.

In order to achieve interoperability, the detailed definition covered by the LNSP implementation flexibility
will  need  to be  known  to  develop  LunaNet compatible  user  terminals.  Therefore,  it  is  expected  that  an
LNSP-specific signal-in-space interface control document (SISICD) will need to be generated and made
available by the LNSP to a {LSIS-TBD-1001} distribution1.

Future  versions  of  this  document  will  further  clarify  the  content  associated  with  LNSP  implementation
flexibility.

In summary, each LNSP that intends to be interoperable with LANS shall:

1.  comply with this document, and

2.  define a SISICD2 that provides definition to the items identified as having LNSP implementation

flexibility in this document, and make it available to a {LSIS-TBD-1001} distribution, and

3.  define user algorithms and models required to process the data specified in the provider specific
SISICD  and  develop  an  example  software  implementation  of  the  user  processing  of  the  raw
navigation bits to obtain the navigation data, including test vectors, and make those available to the
{LSIS-TBD-1001} distribution. A list with minimum requirements on user algorithms and models
as well as test vectors is provided in Appendix G .

This document was written and reviewed by European Space Agency (ESA), the National Aeronautics and
Space Administration (NASA), and the Japan Aerospace Exploration Agency (JAXA).

1.1.  SCOPE
This document defines the interoperable LunaNet standards and specifications for operations on the lunar
surface  and  in  cislunar  space  for  the  LunaNet  1.0  instantiation  of  LunaNet,  at  the  minimum,  the
specifications for interoperability among LNSPs contributing to LANS. It aims to maximize compatibility
across multiple LNSPs at  the user level,  while, at the same time,  leaving sufficient flexibility to enable
tailoring of the services and potential future evolutions. This document, however, is not an interface control
document. As such, it will not provide all interface control level information necessary for complete user
implementation.

1 The actual distribution list and/or the rules for the dissemination of the LNSP-specific SISICD will be clarified in
the future.
2 SISICD in this context means definition of signal characteristics and messages.

LSIS V1.0

29 January 2025

9

1.2.  DEFINITIONS AND ANNEXES

1.2.1. DEFINITIONS

The following definitions are used throughout this document:

TBC: To Be Confirmed. It is used when a value is proposed, but it might change in future versions of this
document.

TBD: To Be Determined. It is  used when something is defined at a conceptual level, but no details are
provided (e.g.: when a parameter is identified, but its numerical value is not yet defined).

TBW: To Be Written. It is used when a paragraph/section is planned to be written in future versions of this
document.

The following conventions apply for the normative specifications in this Recommended Standard:
the words ‘shall’ and ‘must’ imply a binding and verifiable specification.
the word ‘should’ implies an optional, but desirable, specification.
the word ‘may’ implies an optional specification.
the words ‘is’, ‘are’, and ‘will’ imply statements of fact.

a.
b.
c.
d.

1.2.2.  ANNEXES

The following annexes are part of this document (note that these annexes also link to the files marked in
italic):
[Annex1]
[Annex2]

001_LNIS-AD1-Vol-A-Annex1-LDPC-Tables
002_LNIS-AD1-Vol-A-Annex2-LDPC-Submatrices-CSV-README

003_#1_lunanet_ldpc_submatrices_fid0_ind
004_#2_lunanet_ldpc_submatrices_fid0_mat

[Annex3]

005_LNIS-AD1-Vol-A-Annex3-PRN-Spreading-Codes-README

006_#1_GoldCode2046hex210prns.txt
007_#2_l1cp_hex210prns.txt
008_#3_Weil1500hex210prns.txt

LSIS V1.0

29 January 2025

10

2. AUGMENTED FORWARD SIGNAL SPECIFICATIONS

2.1.  INTERFACE DEFINITION
This  section  provides  specifications  of  the  AFS  to  ensure  minimum  interoperability  amongst  LNSPs
providing LANS. The AFS is used within the LANS GNSS-like concept as detailed in LNIS.

The interoperability among different LNSPs is ensured by the compliance with the specifications in this
document,  including  the  Signal-in-Space  Error  (SISE)  specifications,  and  compliance  with  the  Lunar
Reference  System  and  Lunar  Time  System  Standard  described  in  LNIS  AD5  (which  defines  LunaNet
Reference Time aligned with the  Lunar Reference System), and, when  relevant, compliance with LNIS
AD3 (which defines LunaNet messages). The compliance to the SISE (specification LSIS-001) will ensure
that the errors under control of the LNSP (e.g.: orbit prediction error, time synchronization error, satellite
payload biases, etc.) are within a limit, guaranteeing the users that the errors remain within a predefined
envelope.

The following approach is adopted for the messages:

  When a message is defined and/or specified in the same way (e.g.: at bit level) in multiple LunaNet
PNT services (e.g. LANS and P2P), its definition is provided in LNIS AD3, so the related paragraph
in Section 2.5 includes the encapsulation of the message in AFS and refers to LNIS AD3 for the
message detailed definition.

  When a message is implemented differently in AFS with respect to other LunaNet PNT services,

its detailed specification and/or definition are only provided in Section 2.5 of this document.

2.1.1. AFS SIGNAL AND DATA STRUCTURE

The AFS signal defined in this document is a fixed frequency signal consisting of two main components:
one denoted as AFS-I that is spread by a ranging code and modulated by data messages, and AFS-Q that is
spread by a ranging code without any data message (pilot component).

AFS-I  and  AFS-Q  use  Binary  Phase-Shift  Keying  (BSPK)  modulation  and  are  linearly  multiplexed  to
generate the AFS signal as described in 2.3.2.

The AFS-I and AFS-Q components are transmitted using ranging codes defined in Section 2.3.5.

The message structure and data encoding techniques for the data message on AFS-I are defined in Section
2.4 and the content of the message transmitted by the AFS is provided in Section 2.5.

LSIS V1.0

29 January 2025

11

2.2.  AFS SIGNAL-IN-SPACE ERROR DEFINITION

2.2.1. SIGNAL-IN-SPACE ERROR CONCEPT

The  signal-in-space  error  (SISE)  of  an  LNSP  node  is  defined  as  the  combination  of  the  following
instantaneous errors:

1.  SISE position error contribution defined as the difference between:

a.  True position of signal transmission location at the LNSP node (e.g., antenna phase center

or reference point) in the lunar reference system [AD5];

b.  Position as provided in the node’s navigation messages to the user, which is affected by
knowledge errors (combination of position estimation and prediction), antenna reference
point offsets, representation errors (i.e., due to navigation message fitting), and reference
frame conversion errors.

2.  SISE time error contribution represented as a distance and defined as the difference between:

a.  True  time  of  signal  transmission  at  the  LNSP  node  represented  in  LunaNet  Reference

Time (LRT) [AD5];

b.  Time  of  transmission  as  provided  in  the  node’s  navigation  message  to  the  user,
represented  in  LRT.  This  is  affected  by  clock  offsets  due  to  a  combination  of  clock
estimation, prediction and quantization errors; the effects of uncalibrated and unknown
“group”  delays;  and  time  conversion  errors  between  LNSP  Node  Time  (NT),  LNSP
System Time (LST) and LRT.

3.  SISE velocity error contribution defined as the derivative of the SISE position error contribution

(point 1 above).

4.  SISE  time  drift  error  contribution  defined  as  the  derivative  of  the  SISE  time  error  contribution

(point 2 above).

This definition is independent of the orbital characteristics of each LunaNet node and establishes an upper
bound on the error experienced at user level that is the result of the projection of the SISE onto the user-to-
LNSP node direction3. This allows users to derive reliable navigation solutions when using LANS from
different LNSPs. Please refer to Appendix B  for a detailed list of SISE contributors.

2.2.2. SIGNAL-IN-SPACE ERROR MATHEMATICAL DEFINITION

The SISE concept can be defined both for position (SISE pos) and velocity (SISE vel) as shown below:

1.  Signal-In-Space Error for positioning (SISE pos)

= (

) + (

) + (

) + (c

c )  ,

1

,

,

,

  are  the  true  position  and  time  (as  described  in  Section  2.2.1  under  1.a  and  2.a  for
Where
position  and  time,  respectively),  while  the  corresponding  tilde  parameters  represent  the  values
broadcast in the navigation message (as described in Section 2.2.1 under 1.b and 2.b for position and
time, respectively).

3 European GNSS (Galileo), Galileo Service Definition Document, issue 1.2, November 2021, Section 2.3.2.1

LSIS V1.0

29 January 2025

12

2.  Signal-In-Space Error for velocity (SISE vel):

= (

) + (

) + (

) + (c

c )  ,

2

,

,

,

Where
 are the true velocity and clock drift (defined by derivative of items described in Section
2.2.1 under 1.a and 2.a for position and time, respectively), while the corresponding tilde parameters
represent the values broadcast in the navigation message (defined by derivative of items described in
Section 2.2.1 under 1.b and 2.b for position and time, respectively).

2.2.3. SIGNAL-IN-SPACE ERROR SPECIFICATION

LSIS-001: LNSP SISE Position
Each LNSP node that provides AFS shall maintain the SISE position performance shown in Table 1 within
the service volume defined in the specific LNSP SISICD.

Table 1 - LNSP SISE Position

Error

SISE pos

Value
 40 {LSIS-TBC-2001} m (95%) - Calculated as
the  95th  percentile  of
time  series  of
instantaneous SISE values over a 24 {LSIS-TBC-
2002} hours period.

the

Note: LSIS-001 provides maximum SISE position error values for all LNSPs. Different providers
may have tighter requirements to meet the needs of their user community.

LSIS-002: LNSP SISE Velocity
Each LNSP node that provides AFS shall maintain the SISE velocity performance shown in Table 2 within
the service volume defined in the specific LNSP SISICD.

Error

SISE vel

Table 2 - LNSP Velocity

Value

 1 {LSIS-TBC-2003} cm/s (95%) - Calculated
as  the  95th  percentile  of  the  time  series  of
instantaneous SISE values over a 24 {LSIS-TBC-
2004} hours period.

Note: LSIS-002 provides maximum SISE velocity error values for all LNSPs. Different providers
may have tighter requirements to meet the needs of their user community.

LSIS-005: Predicted LNSP SISE Information
Each LNSP node shall provide a predication of the SISE as part of MSG-G4 for both position (SISEpos)
and velocity (SISEvel), as long as AFS is being operationally transmitted.

Note: Through the estimated SISE defined in this specification, using a concept similar to Galileo SISA
(Signal-in-Space Accuracy) or GPS URA (User Range Accuracy), the SISE of different LNSPs is accounted
for consistently at user level.

LSIS V1.0

29 January 2025

13

LSIS-006: Accuracy of Predicted LNSP SISE Position Information
{LSIS-TBW-2001}

LSIS-007: Accuracy of Predicted LNSP SISE Velocity Information
{LSIS-TBW-2002}

2.3.  AFS SIGNAL SPECIFICATIONS

2.3.1. COMPOSITE SIGNAL

FREQUENCY PLAN

2.3.1.1.
LSIS-010: Frequency Band
The frequency band allocated to the AFS signal shall be in S-band between 2483.5 MHz and 2500 MHz.

Note:  This  is  in  line  with  Space  Frequency  Coordination  Group  (SFCG)  recommendation  32-2,  that
identifies the band between 2483.5 MHz and 2500 MHz for “In-situ Lunar based RNSS to Lunar Orbit and
Lunar Surface.”

LSIS-020: Carrier Frequency
The Augmented Forward Signal carrier frequency shall be 2492.028 MHz.

LSIS-030: Carrier Frequency maximum offset
The  maximum  deviation  of  the  transmitted  signal  carrier  frequency  from  the  required  signal  carrier
frequency shall be less than 10 Hz.

Note: The knowledge of the carrier frequency at the user level is bounded by the SISEvel specification.

SIGNAL POLARIZATION

2.3.1.2.
LSIS-040: Signal Polarization
The transmitted AFS signal shall be Right-Hand Circularly Polarized (RHCP).

LSIS-050: Signal Axial Ratio
The transmitted AFS shall preserve an RHCP signal axial ratio of less than 3 dB {LSIS-TBC-2005} over
the antenna beamwidth that covers the service volume defined in LNIS.

CARRIER PHASE NOISE

2.3.1.3.
LSIS-060: Augmented Forward Signal Phase Noise
The phase noise spectral density of the un-modulated carrier shall allow a second-order phase locked loop
with damping of 1 and with 10 Hz one-sided noise bandwidth to track the carrier to an accuracy of 0.04
{LSIS-TBC-2006} radians RMS.

SPURIOUS TRANSMISSIONS

2.3.1.4.
LSIS-070: Maximum In-Band Spurious Transmissions
The  aggregate  EIRP  of  all  unwanted  emissions  (including  discrete  emissions  and  parasitic  emissions)
integrated over the transmit bandwidth of each signal shall not exceed -35 dB {LSIS-TBC-2007} relative to
the total power emitted in the bandwidth specified in 2.3.1.5.

LSIS V1.0

29 January 2025

14

LSIS-080: Maximum Out-Of-Band Emissions
{LSIS-TBW-2003}

CORRELATION LOSSES

2.3.1.5.
Correlation  loss  is  defined  as  the  difference  between  the  power  received  from  the  LNSP  node,  per
component in the bandwidth defined in LSIS-090, and the signal power recovered in an ideal correlation
receiver of the same bandwidth using an exact replica of the waveform within an ideal sharp-cutoff filter
bandwidth centered at the carrier frequency specified in LSIS-020.

LSIS-090: Receiver reference bandwidth
The receiver reference bandwidth centered on the carrier frequency to be considered for the correlation
losses shall be 15.944 MHz {LSIS-TBC-2008}.

LSIS-100: Correlation losses due to payload distortions
For each signal component, the correlation loss due to payload distortions shall be below 0.6 dB {LSIS-
TBC-2009}.

AFS-I AND AFS-Q POWER RATIO

2.3.1.6.
LSIS-103: Power Ratio between AFS-I and AFS-Q components
The relative power sharing between the signal components for the AFS transmit bandwidth shall be per
 is allocated between the power in the AFS-I signal,
Table 3, where the total power
, total power, is given by LSIS-110 and LSIS-120.

  and the power in the AFS-Q signal,

  =

  +

.

Table 3 - Power Ratio between AFS-I and AFS-Q signal components

Signal  Channel  Relative Power for the AFS

Tolerance of Relative Power

AFS

I
Q

transmit bandwidth
50 %
50 %

1 {LSIS-TBC-2010} %
1 {LSIS-TBC-2011} %

2.3.1.7.

LUNAR GLOBAL RECEIVED POWER LEVELS

LSIS-110: Minimum and maximum received lunar global power-on-surface levels
The minimum and maximum received power level of the composite signal at the lunar geoid (as defined
in LNIS AD5), with the following assumptions:

an ideally matched 0 dBi RHCP receiver antenna

  user antenna masking angle on the local horizon of 5 degrees

shall be according to Table 4.

Table 4 - Received Minimum and Maximum Power

Received minimum power
[dBW]
-160 {LSIS-TBC-2012}

Received maximum power
[dBW]
-147 {LSIS-TBC-2013}

LSIS V1.0

29 January 2025

15

RECEIVED POWER LEVEL OUTSIDE THE LNSP-DEFINED SERVICE VOLUME

2.3.1.8.
LSIS-105 [FLEX]: LNSP-Defined service volume
The LNSP shall identify in its SISICD any LNSP-specific service volume on or above the lunar geoid
where its contribution to LANS is provided.

Note: The objective for LANS is to achieve full-service volume per LNIS. However, individual LNSP may
focus on different volumes. LSIS-120 specification applies outside the LNSP-defined service volume.

LSIS-120: Maximum received power levels outside the LNSP-defined service volume
The maximum received power level of the composite signal at the lunar geoid (as defined in LNIS AD5)
outside the LNSP-defined service volume, with the following assumptions:

an ideally matched 0dBi RHCP receiver antenna

  user antenna masking angle on the local horizon of 5 degrees

shall be according to Table 5.

Table 5 - Received Maximum Power

Received maximum power
[dBW]
-141 {LSIS-TBC-2014}

The  signal  may  be  transmitted  outside  the  LNSP-defined  service  volume  to  enable  opportunistic  use.
However, its power shall be within the maximum defined in Table 5 in order to minimize the impact of
multiple access interference towards the user receiver.

[Text below is {LSIS-TBC-2015}]

If LSIS-120 is applicable, power-on-surface level shall override the maximum values specified by LSIS-
110. The applicability of this specification (LSIS-120 and the associated power level in Table 5) are to be
determined  by  coordination  among  LunaNet  administrative  partners.  In  the  absence  of  LSIS-120,  the
power-on-surface shall not exceed the maximum values specified by LSIS-110 (and the associated power
levels in Table 4).

Note: It is therefore expected that LNSPs will include provisions to be able to adhere to LSIS-110 even
outside the LNSPs’ service volume in case of need (e.g. to either decrease the power or interrupt signal
transmission if necessary).

[Text above is {LSIS-TBC-2015}]

2.3.2. MODULATION

The Augmented Forward Signal is composed of two components, one in-phase and one quadrature, called
respectively data and pilot. Both components use Binary Phase Shift Keying (BPSK) modulation, with chip
rate of n × 1.023 (Mchip/s); the data component consists of a BPSK (1) modulation (where n = 1), while
the quadrature component is a BPSK (5) modulation (where n = 5). Figure 2 provides a generic view of the
AFS generation.

LSIS V1.0

29 January 2025

16

LSIS-125: Signal multiplexing
The AFS components shall be linearly multiplexed and modulated according to Figure 2

Figure 2: Modulation Scheme for AFS Signal

LSIS-130: Signal component generation
The AFS components shall be generated according to the following relation:

( ) =

( ) cos(2

)

( ) sin(2

),

where,

( ) =

, and

( ) =

2

The  I  channel  is  modulated  by  the  I  channel  spreading  code  (
(
). The Q channel is modulated by a tiered spreading code (
of the parameters used in the relations above.

)  and  the  data  symbol  sequence
). Table 6 provides a description

LSIS V1.0

29 January 2025

17

Table 6 - Description of Signal Parameters

Parameter

Description
AFS carrier frequency as defined by LSIS-
020.
Data channel (AFS-I) spreading code, as
described in Section 2.3.5.
Data channel symbol sequence as described in
Section 2.4.
Pilot channel (AFS-Q) tiered spreading code,
as described in Section 2.3.5.
Data channel (AFS-I) power, derived from
LSIS-103, LSIS-110 and LSIS-120.
Pilot channel (AFS-Q) power, derived from
LSIS-103, LSIS-110 and LSIS-120.

LSIS-140: signal ranging code chip-rate and symbol-rate
The LNSP nodes shall transmit the AFS signal components with the ranging codes chip rates and symbol
rates stated in Table 7.

Table 7 - AFS Chip Rates and Symbol Rates

Component  Ranging code chip-rate [Mchip/s]  Symbol-rate [symbols/s]

I
Q

1.023
5.115

500
No data (pilot component)

2.3.3. LOGIC LEVELS

LSIS-150: Logic levels for the code bits
The correspondence between the logic level code bits used to modulate the signal and the
signal level shall be according to the values stated in Table 8.

Table 8 - Logical to Signal Level Assignment

Logic level  Signal level

1
0

-1.0
1.0

2.3.4. TRANSMITTED SIGNALS COHERENCY

LSIS-160: Primary code / data coherency
The start of each data symbol shall coincide with the start of a primary code sequence, with a maximum
tolerance of 0.5 ns {LSIS-TBC-2016}.

Note: This means that the number of primary code chips per each data symbol is equal to the 'primary code
chip-rate / data symbol-rate', according to the respective signal component (AFS-I).

LSIS-170: Secondary Code / Primary Code Coherency
The  start  of each  secondary  code  chip  shall  coincide  with the  start  of  a  primary  code sequence, with  a
maximum tolerance of 0.5 ns {LSIS-TBC-2017}.

Note: The number of primary code chips per each secondary code chip is equal to the 'primary code chip-
rate / secondary code chip-rate', according to the respective signal component (AFS-Q).

LSIS-171: Tertiary Code / Secondary Code Coherency

LSIS V1.0

29 January 2025

18

The  start  of  each  tertiary  code  chip  shall  coincide  with  the  start  of  a  secondary  code  sequence,  with  a
maximum tolerance of 0.5 ns {LSIS-TBC-2017a}.

Note: The number of secondary code chips per each tertiary code chip is equal to the 'secondary code chip-
rate / tertiary code chip-rate', according to the respective signal component (AFS-Q).

LSIS-172: Tiered Code Coherent Generation
All elements of the tiered code (i.e., primary, secondary and tertiary) shall be generated coherently, such
that no chip slips occur throughout each iteration of the composite code generation function.

Note: code generation function as shown in Figure 3.

LSIS-180: Code-Code (Data/Pilot) Coherency
The code-code coherency between the data primary code and the pilot primary code shall be less than
0.5ns {LSIS-TBC-2018}.

Note: Code-Code Coherency is the time difference measured between code delays of two signal
components within the same signal.

LSIS-190: Code/Carrier Phase Coherency
The maximum of the difference between code phase and carrier phase on any single signal component in
any 8-hour period at the phase center of the LNSP transmit antenna shall be less than 0.13 ns (1-sigma)
{LSIS-TBC-2019}.

2.3.5. SPREADING CODES CHARACTERISTICS

CODE LENGTHS

2.3.5.1.
The  ranging  codes  are  built  from  so-called  primary,  secondary  and  tertiary  codes  using  a  tiered  codes
construction described in paragraph 2.3.5.2

LSIS-200: Codes Length and Duration
The code lengths to be used for each signal component shall be according to Table 9.

Table 9 - Code Lengths

AFS
Component

I
Q

Primary
code
period
[ms]
2
2

Secondary
code
period
[ms]
N/A
8

Tertiary
code
period
[ms]
N/A
12000

Code length [chips]

Primary

Secondary  Tertiary

2046
10230

N/A
4

N/A
1500

LSIS-210: Code-Code (Data/Pilot) Synchronicity
The data primary code and data symbols and the pilot primary, secondary and tertiary code shall be
transmitted according to Figure 3. The secondary code pattern shown in this figure represents S0 (Table
10) and is included here as an example.

LSIS V1.0

29 January 2025

19

Figure 3: Data and Pilot Channel Code and Data Synchronization

LSIS-220: Tertiary Code Synchronization
The start of the tertiary code sequence shall be synchronized with the start of the AFS-I data frame structure.

Note: The code/code coherency specified by LSIS-180 drives the accuracy of the synchronization. AFS-I
data frame structure is specified by LSIS-310.

PILOT CHANNEL (AFS-Q) TIERED CODES GENERATION

2.3.5.2.
Long  spreading  codes are typically  generated  by a  tiered code construction,  whereby a secondary code
sequence is used to modify successive repetitions of a primary code. The AFS pilot channel consists of
three codes, a primary spreading code, a secondary code and a tertiary code, as shown Figure 4. The primary
code is of length
and clock rate fc (same as chip rate), the secondary code is of length NS and chip rate
fcs; and the tertiary code is of length NT and a chip rate fcT (values in alignment with Table 7 and Table 9).

chips is also called a primary code epoch (or repeat interval), as shown Figure 4. In
The duration of
logical representation, the secondary code chips are sequentially modulo 2 added with the primary code,
with one chip of the secondary code per primary code period, and then the tertiary code is modulo 2 added
onto the modulo 2 sums of the primary and secondary tiered code.  Likewise, there is one tiered (primary
+ secondary) code period per tertiary code chip, to produce the final tiered AFS-Q channel ranging code,

. This tiered code has a length of   ×

×

primary code spreading chips.

LSIS V1.0

29 January 2025

20

Figure 4: Tiered Codes Generation with Secondary and Tertiary Codes

There  are  1500  tertiary code  symbols  that  span  the  12  second  data  frame  and  provide  rapid  time
dissemination (within  12 seconds),  and  robust frame synchronization after  synchronization to the tiered
primary + secondary code,

.

2.3.5.3.

SPREADING CODES

2.3.5.3.1. DATA CHANNEL (AFS-I) SPREADING CODE DEFINITION
LSIS-221: Data Channel (AFS-I) Primary Code Definition
The data channel (AFS-I) primary code shall be a 2047 chip Gold sequence short-cycled to length 2046, as
described in Appendix C and [Annex3] (hexadecimal representation).

LSIS V1.0

29 January 2025

21

2.3.5.3.2.  PILOT CHANNEL (AFS-Q) SPREADING CODE DEFINITION

LSIS-222: Pilot Channel (AFS-Q) Primary Code Definition
The pilot channel (AFS-Q) primary code shall be a Weil sequence of length 10230 which is derived from
a  Legendre  sequence  L(t)  of  length  10223  as  described  in  Appendix  D  and  [Annex3]  (hexadecimal
representation).

Note-1: The Weil and insertion indices (and the resulting hexadecimal codes) provided in Appendix D (and
the csv file [Annex3]) are currently {LSIS-TBC-2020}. The codes provided in this release of the document
are included to facilitate test and integration efforts only. It is therefore recommended for implementers to
support a flexible code configuration, enabling code changes to be applied through configuration updates.

LSIS-223: Pilot Channel (AFS-Q) Secondary Code Definition
The pilot channel (AFS-Q) secondary code shall be one of the four possible sequences shown in Table 10.

Table 10 - Secondary Code Definitions

Secondary Code Identifier

Secondary Code (binary)

S0
S1
S2
S3

1110
0111
1011
1101

LSIS-224: Pilot Channel (AFS-Q) Tertiary Code Definition
The pilot channel (AFS-Q) tertiary code shall be a Weil sequence of length 1500 which is derived from a
Legendre sequence L(t) of length 1499 as described in Appendix E  and [Annex3] (hexadecimal
representation).

2.3.5.4.

CODE ASSIGNMENT TO SATTELLITES / LNSP

LSIS-260: Matched-Codes Assignment Per LNSP Node
Each LNSP node shall be linked to a single LNSP node identifier which has a unique combination of code
sequences assigned as defined in Table F- 1 (Appendix F ).

Note-1:  The  assignments  between  the  LNSP  node  identifiers  and  combination  of  code  sequences  are
currently {LSIS-TBD-2001}, and codes provided in this release of the document are included to facilitate
test  and  integration  efforts  only.  The  assignment  of  codes  to  a  particular  LNSP  node  identifier  will  be
completed in a future release, to allow for optimization of the assignments between primary, secondary,
and tertiary codes as well as the relevant phasing of the tertiary code.

Note-2:  Interim  assignments  for  the  first  12  codes  are  provided  in  Table  11  for  integration  and  test
purposes.

Note-3: This document does not address the assignment of LNSP node identifiers to particular LNSP nodes.

LSIS V1.0

29 January 2025

22

Table 11 – Interim Test and Integration Code Assignments

LNSP Node
Identifier

Data Channel
(AFS-I)

Primary PRN

1
2
3
4
5
6
7
8
9
10
11
12

1
2
3
4
5
6
7
8
9
10
11
12

Pilot Channel
(AFS-Q)

Secondary
Code
S0
S1
S2
S3
S0
S1
S2
S3
S0
S1
S2
S3

Tertiary
PRN
1
2
3
4
5
6
7
8
9
10
11
12

Primary
PRN
1
2
3
4
5
6
7
8
9
10
11
12

Tertiary PRN
Phase Offset
0
0
0
0
0
0
0
0
0
0
0
0

LSIS V1.0

29 January 2025

23

2.4. AFS NAVIGATION MESSAGE FORMAT SPECIFICATION

2.4.1. GENERAL NAVIGATION MESSAGE STRUCTURE

LSIS-300: Spare Bits Definition
When not specified otherwise, the spare bits in the message structure shall be filled with a sequence of
alternating zeros and ones starting with zero on the MSB.

LSIS-310: Message Frame Structure
Each frame shall be composed of:

1)

the uncoded synchronization pattern (SP)

2) one subframe (called Subframe Block 1, SB1) that contains the Time of Interval (TOI) and the

Frame Identifier (FID)

3) and subsequent subframes that depend on the specific FID value.

See Figure 5for a representation of the message frame structure.

Figure 5: Navigation Message Generic Structure

LSIS V1.0

29 January 2025

24

BIT AND BYTE ORDERING CRITERIA

2.4.1.1.
LSIS-320: Bit and Byte Ordering Criteria
All data values shall be encoded using the following bit and byte ordering criteria (as shown in Figure 6):

For numbering, the most significant bit/byte is numbered as bit/byte 0
For bit/byte ordering, the most significant bit/byte is transmitted first

Figure 6: Bit and Byte Ordering

SYNCHRONIZATION PATTERN

2.4.1.2.
LSIS-330: Synchronization Pattern (SP)
The synchronization pattern (SP) shall be according to Table 12.

Note: The synchronization pattern allows the receiver to achieve synchronization to the frame boundary.
The sequence is chosen for its favorable autocorrelation properties4.

Table 12 - Synchronization Pattern

Number of symbols

Hexadecimal pattern

68

CC63F74536F49E04A {LSIS-TBC-2021}

LSIS-340: Uncoded Synchronization Pattern
The SP shall not be encoded.

2.4.2. SUBFRAME 1 MESSAGE SPECIFICATION

SUBFRAME 1 ENCODING

2.4.2.1.
Subframe 1 data is channel encoded using a Bose–Chaudhuri–Hocquenghem (BCH) code (51, 8). The eight
Least Significant Bits (the rightmost  bits  1 - 8) of the nine-bit SB1 field are encoded using  a generator
polynomial of 763 (octal). This code generator is conceptually described in Figure 6using an 8-stage linear
shift register generator. The data bits are loaded into the generator, bit 1 first, as initial conditions of the
registers, so that the starting condition has bit one in stage 8 and bit 8 in stage 1. The generator is then
shifted 51 times to generate 51 encoded symbols. The MSB of SB1 (bit 0) shall be modulo-2 added to the
51 encoded symbols and it shall also be prepended as the MSB of the 52-symbol message. An example of
a BCH(51,8) encoded SB1 is provided in Figure 7.

4 Sükrü Ekin Kocabas, Abdullah Atalar, “Binary Sequences With Low Aperiodic Autocorrelation for
Synchronization Purposes,” IEEE Communications Letters, Vol. 7, No. 1, January 2003

LSIS V1.0

29 January 2025

25

Figure 7: Subframe 1 BCH Encoding

Figure 8: Example of BCH(51,8) encoder for subframe-1 (SB1)

The SB1 data field and encoded field are provided also in hexadecimal format (on the righthand side). Note
that three ones (i.e., [0 0 0]) are padded to the left-hand side of the SB1 data to enable a h hexadecimal
representation. The following provides an example decoding technique to decode the Subframe 1 encoded
data.  The  52  user-equipment-received  soft  decisions  are  stored  as  sign/magnitude  and  correlated,
respectively,  with  the  52  symbols  of  a  code  word  hypothesis  corresponding  to  MSB  =  0.  (An  LNSP
transmitted 0 is expected to produce a sign of 0). For each soft decision, the correlation computation adds
the magnitude if the sign agrees with the code word hypothesis and subtracts the magnitude otherwise. The
correlation  computation  is  repeated  for  all  256  code  word  hypotheses.  The  decision  on  the  eight  LSBs
corresponds  to  the  code  word  hypothesis  producing  the  largest  absolute  value  of  the  correlation.  The
decision on the MSB is 0 if this largest correlation is positive and 1 otherwise.

LSIS V1.0

29 January 2025

26

SUBFRAME 1 (SB1) DATA SPECIFICATION

2.4.2.2.
LSIS-410: Subframe 1 Data Definition
The fields of the SB1 data shall follow the description in Table 13.

Table 13: Subframe 1 Fields Specification

Parameter
FID
TOI

Definition
Frame ID
Time of interval

2
7
Note: The FID is used to identify the structure of the frame as described in 2.4.3. The TOI is used to
reconstruct the time at the user level as described in 2.5.5.

N/A
N/A

Unit
dimensionless
dimensionless

Values
0…3
0…99

Bits  Scale factor

LSIS-415: Subframe 1 Bit Assignment
Subframe 1 shall use the two MSBs to represent the FID information and the remaining seven LSBs to
represent the TOI that corresponds to the NT (Node Time representing the time of the node clock) epoch at
the start (leading edge) of the next 12-second frame as shown in Table 14 .

Table 14: Bit Allocation for Subframe 1

Subframe 1

FID

TOI

2

7

l
a
t
o
T

)
s
t
i

b
(

9

2.4.3. FRAME STRUCTURE

The Frame Identifier (FID) field is used to define different types of frames and allows for implementation
of different schemes, including:

  different type of encoding of the “Other Subframes” other than SB1,

  different type and number of subframes.

LSIS-420: Frame Structure Identification
The frame structure shall be identified using the Frame Identifier (FID) field in SB1 in line with the
values in Table 15.

Table 15: Frame Structure Identification

FID
0

1 to 3

Definition
Frame structure including
SB2, SB3 and SB4
Reserved for future use

Reference
Paragraph
2.4.3.1
Future Use

FRAME ID 0 MESSAGE STRUCTURE

2.4.3.1.
In  addition  to  the  synchronization  pattern  (SP)  and  SB1  that  maintain  the  structure  defined  in  2.4.2,  3
additional subframe types are specified within this frame type (i.e., FID0).

Subframe 2 (SB2) is broadcast in every frame and its structure is not changing, as described in 2.4.3.1.5.
This subframe contains, among others, clock and ephemeris data.

LSIS V1.0

29 January 2025

27

Subframe 3 (SB3) is a variable data frame that will contain different content in each frame. The specific
information broadcast is identified by the subframe identifier as described in 2.4.3.1.7.

Subframe 4 (SB4) is  a  variable  data  frame  that  will  contain  different  content  in  each  frame.  The data
potentially  supports services  other  than  the  navigation  service. The  specific  information  broadcast  is
identified by the subframe identifier as described in 2.4.3.1.8.

Note for the current version of this document: The structure of the frame is meant to ensure dissemination
of the mandatory and optional messages as defined in LNIS. At the same time, the proposed concept leaves
flexibility to the LNSPs to implement their specific concepts, so some aspects (e.g.: dissemination structure,
dissemination frequency,  etc.)  are  not  specified  in  this  version  of the  document. A  clear  example  is  the
possibility to disseminate some messages in SB3 and/or SB4 and the possibility to use multiple frames to
disseminate the complete message (e.g.: the almanac may be disseminated using multiple SB3 and/or SB4
over multiple frames). The next version of this document is expected to contain additional specifications to
guide LNSPs towards a common concept and ensure interoperability.

LSIS-FID0-430: Frame ID 0 Structure Identification

The frame structure identified with FID equal to zero (i.e., FID0) shall be in accordance Figure 9.

Figure 9: Frame Structure of Frame ID Equal to Zero

2.4.3.1.1. SUBFRAME ENCODING (NON-SUBFRAME 1)
LSIS-FID0-440: Subframe 2 Encoding
The 1200 bits of SB2 data, including CRC, shall be encoded using 1
Section 2.4.3.1.2.

2 LDPC codes as described in

LSIS V1.0

29 January 2025

28

LSIS-FID0-450: Subframe 3 Encoding
The 870 bits of SB3 data, including CRC, shall be encoded using 1
Section 2.4.3.1.2.

2 LDPC codes as described in

LSIS-FID0-460: Subframe 4 Encoding
The 870 bits of SB4 data, including CRC, shall be encoded using 1
Section 2.4.3.1.2.

2 LDPC codes as described in

2.4.3.1.2.  LDPC ENCODING
Subframe 2, Subframe 3, and Subframe 4 are separately encoded using rate ½ LDPC codes. Subframe 2
has a total of 1200 bits consisting of 1176 bits for data and 24 bits for CRC. Subframe 3 and subframe 4
each have a total of 870 bits consisting of 846 bits for data and 24 bits for CRC. As a result of the rate ½
LDPC encoding, there are 2400 symbols (coded bits) for Subframe 2 and 1740 symbols for Subframe 3 and
subframe 4 as described in Figure 9.

The  LDPC  encoder  structure  is  based  on  a parity-check  matrix  H(m,  n)  of  m rows  and  n columns.  For
Subframe 2, m = 1200, n = 2400 and for Subframe 3, m = 870, n = 1740. H(m, n) is further decomposed
into 6 submatrices A, B, C, D, O, and I as shown in Figure 10. Each element of matrix H(m, n) is either a
value of “0” or “1”. “O” represents a matrix of all zeros with the designated size. “I” represents an identity
matrix with the designated size.

Tables in [Annex1] Sections 2.1.1, 2.1.3, 2.1.4, 2.1.5, and 2.1.2 define the coordinates of elements with
value “1” in each of the submatrices A, B, C, D, and B-1, respectively, for Subframe 2.
Tables in [Annex1] Sections 2.2.1, 2.2.3, 2.2.4, 2.2.5, and 2.2.2 define the coordinates of elements with
value “1” in each of the submatrices A, B, C, D, and B-1, respectively, for Subframe 3 and Subframe 4.

The rate ½ LDPC encoder shall use the given matrices A, B-1, C, and D defined in Annex-1 to generate the
encoded symbols using the following algorithm:
t

p1
p2

t = B-1
t

t

t

1

where,

s = Subframe 2, Subframe 3, or Subframe 4 data with “filler bits”, where,

o
o

for Subframe 2, no filler bits are needed
for Subframe 3 and Subframe 4, 10 filler bits are needed (i.e. 10 zero bits appended to the
end of the data)

[ ]t indicates transpose,
and elements of matrices p1 and p2 are modulo-2 numbers.

The encoded symbols for broadcast are comprised of (s*;p*) where,

s* is the systematic portion of the codeword with the first z bits punctured and the filler bits also
punctured,
z is the lifting factor multiplied by two, where,

z = 240 for Subframe 2
z = 176 for Subframe 3 and Subframe 4

and p* = (p1;p2) the combined parity bits appended until the subframe reaches its desired symbol
count. The remaining parity bits are punctured and not transmitted across the channel.

LSIS V1.0

29 January 2025

29

When receiving the encoded symbols, the punctures must be restored prior to decoding the message. The
punctured symbols (the first z bits of the systematic portion, filler bits, and the remainder of the parity bits)
can be treated as erasures (i.e. if using log-likelihood ratios for a belief-propagation LDPC decoder, the
values can be initialized as zero). A successfully decoded codeword will have the representation (s;p1;p2),
where the systematic portion s is the transmitted data.

A

C

m rows
(5040/3696)

0

I

n rows
(6240/4576)

B

D

…

…

…

…

…

…

…

…

…

…

…

…

…

…

…

…

480/352

4560/3344

…

…

…

…

…

…

…

…

1200/880

480/352

4560/3344

Figure 10: LDPC Submatrices

2.4.3.1.3.  CHECKSUM VIA CYCLIC REDUNDANCY CHECK
The checksum, which employs a Cyclic Redundancy Check (CRC) technique, is used to detect the reception
of corrupted data. For a detailed description of the CRC mechanism, please refer to Appendix G .

LSIS-FID0-465: Information guarded by CRC
The CRC shall guard all information in SB2, SB3 and SB4 including spare-bits (when applicable).

LSIS-FID0-467: Checksum Field Generator Polynomial
For the SB2, SB3 and SB4 a CRC of 24 bits shall be generated from the following generator polynomial:

( ) = (1 + )

( )

where P(X) is the primitive and irreducible polynomial with the following definition:
+ 1

( ) =

+

+

+

+

+

+

+

+

+

LSIS-FID0-469: Checksum Field Computation
The CRC shall be composed of a sequence of 24 parity bits
For any   from 1 to 24,

 is the coefficient of

 in ( ),

 with the following values.

Where:

( ) is the remainder of the binary polynomial algebra division of the polynomial m( )

 by  ( )

LSIS V1.0

29 January 2025

30

( ) =

+ +

+

+

   with

,

,  ...

 the  sequence  of  k-bits

information to be protected by the CRC, and

 as the MSB.

2.4.3.1.4.  SUMMARY OF FRAMEID 0 ENCODING
Table 16 provides a summary of the encoding parameters for the different subframes.

Table 16 - Frame ID=0 Bits and Symbols Allocation to Subframes

Subframe
Subframe 2
Subframe 3
Subframe 4

bits

coding
1200  LDPC
870  LDPC
870      LDPC

coding
rate

symbols

  1/2
  1/2
1/2

2400
1740
1740

2.4.3.1.5.  INTERLEAVING
The 5880 symbols representing all subframe types except SB1 are interleaved, after encoding, using a block
interleaver for improved performance in a fading channel. The block interleaver is conceptually described
using a two-dimensional array of 60 rows and 98 columns. Symbols are written first (MSB first) into the
interleaver from left to right starting at Row 1. After Row 1 is filled, Row 2 is filled from left to right and
this  process  continues  until  the  5880th  symbol  (LSB of  LDPC encoded  symbol of  the  last  subframe)  is
written into the rightmost cell of the last (60th) row. Once all 5880 symbols are written into the array, the
symbols are sequentially read out of the array, for broadcast to user, from top to bottom starting at Column
1. After reading out the last symbol of the 60th row in Column 1, Column 2 symbols are read out from top
to bottom and this process continues until the last symbol in the 60th row of the last column (98th) is read
out.

LSIS-FID0-470: Symbol interleaving parameters
All the symbols of the frame, except for the synchronization pattern (SP) and SB1 symbols, shall be
interleaved according to the parameters in Table 17.

Table 17 - Interleaving Parameters

Parameters

Block interleaver size (symbols)
Block interleaver dimensions (n columns x k rows)

Values

5880
60 x 98

2.4.3.1.6.  SUBFRAME 2 MESSAGE SPECIFICATIONS
LSIS-FID0-500: Subframe 2 Dissemination
The SB2 shall be broadcast in every frame.

LSIS-FID0-510: Subframe 2 Structure
The SB2 structure shall be identical in every frame.

LSIS-FID0-520: Subframe 2 Fundamental Type of Data Content
SB2 shall contain at least the following data:

  Clock and ephemeris data (CED) (MSG-G4, see also 2.5.3)
  Time of transmission (ToT) (MSG-G8, including Week Number (WN) and Interval Time

of Week for which the definition is provided in 2.5.1)

  Health and Safety (MSG-G2, including LNSP node health status, see also 2.5.2)

LSIS V1.0

29 January 2025

31

  Time Conversions (MSG-G30)
  Cyclic Redundancy Check (CRC) (see Appendix G )

LSIS-FID0-530: Subframe 2 Layout
The SB2 layout shall be according to Table 18.

Table 18 - Bit Allocation for Subframe 2

Subframe 2

Data

1176

CRC

24

l
a
t
o
T

)
s
t
i
b
(

1200

LSIS-FID0-540 [FLEX]: Subframe 2 LNSP Specific Data
Additional data specified by the LNSP may be broadcast in SB2 in addition to the mandatory data.

LSIS-FID0-550: Subframe 2 Mandatory Spare Bits
{LSIS-TBC-2022} bits shall be kept as spare for future use.
Note:  Spare  bits  shall  be  positioned  before  the  CRC  and  are  guarded  by  the  CRC.  Depending  on  the
definition of the data field in SB2 the number of spare bits might vary and potentially might be removed
entirely.

2.4.3.1.7.  SUBFRAME 3 MESSAGE SPECIFICATIONS
The  SB3  is  a  dynamic  subframe  that  will  have  different  structures  depending  on  the  SB3  type.  This
subframe may be used for multiple purposes, including specific custom messages from the LNSP* that are
not specified in this document (e.g.: proprietary messages). Proprietary messages are not interoperable and
require the LNSP to establish a SISICD for user implementation (refer to Section 2.1).

*Note:  This  definition  is  a  functional  specification  intended  to  provide  the  LNSP  with  implementation
flexibility.

LSIS-FID0-560: Subframe 3 Layout
The SB3 layout shall be according to Table 19.

Table 19 – Bit Allocation for Subframe 3

Subframe 3

SB3 type

Data

4 or 6
{LSIS-TBC-
2023}

840 or 842 {LSIS-
TBC-2023}

CRC

24

l
a
t
o
T

)
s
t
i
b
(

870

LSIS-FID0-570: Subframe 3 Dynamic Data Content
The SB3 data content shall be identified by the SB3 type field.

LSIS-FID0-580: Subframe 3 Fundamental Type of Data Content
The following data shall be disseminated using SB3:

  M(ultiple) Orbit Almanac (note that the almanac data for the full constellation is expected

to be disseminated using multiple SB3) (MSG-G5, see also 2.5.4).

LSIS V1.0

29 January 2025

32

  LunaSAR return link message (covering both MSG-S19 and MSG-S20 considering LvL1
and  LvL2,  with definition provided in 2.5.11) – Note that while  considered mandatory,
these messages are placeholders only and not a part of LunaNet 1.0.

  Alerts messages (MSG-G24), with definition provided in Section 2.5.15. – Note that while
considered mandatory, these messages are placeholders only and not a part of LunaNet
1.0.

  Coordinate Frame Conversions (MSG-G32)

Note: The data listed in this specification does not need to be disseminated every frame, the frequency of
the  dissemination  of  the  data  is  not defined  in  this  document.  The  data  of  a  specific  message  might  be
disseminated over multiple frames.

LSIS-FID0-590: Subframe 3 Optional Type of Data Content
The following data should be disseminated using SB3:

  M(ultiple) Antenna Properties (MSG-G3)
  GNSS Augmentation (MSG-G23)
  Maneuver (MSG-G10)
  S(ingle) Attitude State/ Ephemeris (MSG-G11)
  Conjunction (MSG-G14) – not LunaNet 1.0
  Augmentation differential Corrections (MSG-G31)
  Custom messages defined by the LNSP.

2.4.3.1.8.  SUBFRAME 4 MESSAGE SPECIFICATIONS
The  SB4  is  a  dynamic  subframe  that  will  have  different  structures  depending  on  the  SB4  type.  This
subframe may be used for multiple purposes, including specific custom messages from the LNSP that are
not  specified  in  the  Lunar  Signal-in-Space  Recommended  Specification  (e.g.:  proprietary  messages*).
Proprietary  messages  are  not  interoperable  and  require  the  LNSP  to  establish  a  SISICD  for  user
implementation (refer to Section 2.1).

*Note:  This  definition  is  a  functional  specification  intended  to  provide  the  LNSP  with  implementation
flexibility

LSIS-FID0-600: Subframe 4 Layout
The SB4 layout shall be according to Table 20.

Table 20 – Bit Allocation for Subframe 4

Subframe 4

SB3 type

Data

4 or 6
{LSIS-TBC-
2024}

840 or 842 {LSIS-
TBC-2024}

CRC

24

l
a
t
o
T

)
s
t
i
b
(

870

LSIS-FID0-610: Subframe 4 Dynamic Data Content
The SB4 data shall be identified by the SB4 type field.

LSIS-FID0-620: Subframe 4 Fundamental Type of Data Content

LSIS V1.0

29 January 2025

33

The following data shall be disseminated using SB4:

  LunaNet Network Access Information (MSG-G1).

Note: The data listed in this specification does not need to be disseminated in every frame, the frequency
of the dissemination of the data is not defined in this document. The data of a specific message might be
disseminated over multiple frames.

LSIS-FID0-630: Subframe 4 Optional Type of Data Content
The following data should be disseminated using SB4:

  M(ultiple) Antenna Properties (MSG-G3)
  M(ultiple) Orbit Almanac (note that the almanac data for the full constellation is expected

to be disseminated using multiple SB4) (MSG-G5, see also 2.5.4)

  Maneuver (MSG-G10)
  S(ingle)Attitude State/ Ephemeris (MSG-G11)
  GNSS Augmentation (MSG-G23) – not LunaNet 1.0
  User Schedule Notice (information for communication service) (MSG-G28)
  Maplet (including updates of lunar maps or lunar DEM) (MSG-G15) – not LunaNet 1.0
  Ancillary info (MSG-G17)
  Conjunction (MSG-G14) – not LunaNet 1.0
  Science data (MSG-G25) – not LunaNet 1.0
  Acknowledge- of non-SAR MSG (MSG-G22) – not LunaNet 1.0
  UIS Response (MSG-G27) – not LunaNet 1.0
  FF Commands (MSG-G29) – not LunaNet 1.0
  Augmentation Differential Corrections (MSG-G31)
  Coordinate Frame Conversions (MSG-G32)
  Custom messages defined by the LNSP*.

Note: The data listed in this specification does not need to be disseminated in every frame, the frequency
of the dissemination of the data is not defined in this document. The data of a specific message might be
disseminated over multiple frames.

Note: Almanac data (MSG-G5) needs to be disseminated using SB3, however, to improve the latency or
allow dissemination of complementary almanacs (for other LNSPs), SB4 can optionally be used for this
purpose.

*Note:  This  definition  is  a  functional  specification  intended  to  provide  the  LNSP  with  implementation
flexibility

2.4.3.1.9.  ALLOCATION OF LUNANET MESSAGES TO SUBFRAMES
This section contains the mapping between LunaNet messages (see Table 4 of LNIS) with the subframes
specified in this document, as captured by Table 211. In line with LNIS, a label is used to identify which
category  applies  to  that  message/service  combination  within  the  available  AFS  bandwidth:  F  =
Fundamental, meaning it shall be broadcast by the LNSP; O = Optional, meaning it might be broadcast by
the LNSP; and C = Comm, meaning it may be transmitted on AFS to facilitate LunaNet services. M and S
in front of MSG title words in Table 21 refers respectively to multiple and single, indicating whether the
message is applicable to the LNSP node sending the message (using “S”) or when the LNSP node sends
the message applicable to multiple LNSP nodes (using “M”). This table also identifies applicability to LNIS

LSIS V1.0

29 January 2025

34

V1.0. Messages that are not part of LNIS V1.0 are included as placeholder only and will not be specified
as part of LNIS V1.0.

Table 21 - LNIS Message Allocation to AFS Subframes

LunaNet Interoperability Specifications [AD.1]

LSIS (this document)

MSG ID  MSG Title

Category  Subframe

Notes

MSG-G1

LunaNet Network Access Information

MSG-G2

Health and Safety

MSG-G3  MAntennaProperties

MSG-G4

Sorbit Ephemeris & clock correction
(Clock and Ephemeris Data – CED)

MSG-G5  MOrbit Almanac

MSG-G8

Time of transmission (ToT)

MSG-G10  Maneuver

MSG-G11

SAttitude State/ Ephemeris

MSG-G14  Conjunction

MSG-G15  Maplet

MSG-G17  Ancillary info

MSG-S19  Acknowledge- of SAR - LvL1

MSG-S20  Acknowledge- of SAR - LvL2

MSG-G22  Acknowledge- of non-SAR MSG

MSG-G23  GNSS Augmentation

MSG-G24  Detection Alert

MSG-G25

Science

MSG-G27  UIS Response

MSG-G28  User Schedule Notice

MSG-G29

FF Commands

MSG-G30

Time Conversions

F

F

O

F

F

F

O

O

O

O

O

F

F

O

O

F

C

C

C

C

F

SB4

SB2

SB3 and/or SB4

SB2

periodic

periodic
{LSIS-
TBD-
2002}

periodic

SB3 and/or SB4  periodic
SB1, SB2 and
SB3

periodic

SB3 and/or SB4  Ad-hoc

SB3 and/or SB4  Ad-hoc

SB3 and/or SB4  Ad-hoc

SB4

SB4

SB3

SB3

SB4

Ad-hoc

Ad-hoc

Ad-hoc

Ad-hoc

Ad-hoc

SB3 and/or SB4  Ad-hoc

SB3

SB4

SB4

SB4

SB4

SB2

Ad-hoc

Ad-hoc

Ad-hoc

Ad-hoc

Ad-hoc

Periodic

MSG-G31  Augmentation Differential Corrections  O

SB3 and/or SB4  Ad-hoc

MSG-G32  Coordinate Frame Conversions

F

SB3 and/or SB4  Periodic

LNIS
V1.0
yes

yes

yes

yes

yes

yes

yes

yes

no

no

yes

no
yes
(see
Note-
1)
no

no

no

no

no

yes

no

yes

yes

yes

Note-1: LunaNet 1.0 item, limited to reserving a bit allocation for LunaSAR Return Messages that are to
be disseminated by LNSP nodes via AFS.

LSIS V1.0

29 January 2025

35

LSIS V1.0

29 January 2025

36

2.5.  AFS MESSAGES AND DATA CONTENT
This chapter describes the messages and data content applicable to AFS, following the logic described in
Section 2.1, and recalled here:

  When a message is defined and/or specified in the same way (e.g.: at bit level) in multiple LunaNet
PNT services (e.g. LANS and P2P), its definition is provided in LNIS AD3, so the related paragraph
in Section 2.5 includes the encapsulation of the message in AFS and refers to LNIS AD3 for the
message detailed definition.

  When a message is implemented differently in AFS with respect to other LunaNet PNT services,

its detailed specification and/or definition are only provided in Section 2.5 of this document.

Note for the current version of this document: the content of many of the messages is currently undefined
and it is expected to be specified in future releases of this document. It is important to note that different
messages might follow different approaches. In line with the descriptions in Section 1 of this document, the
following specification categorizations apply to messages:

1)  Precise specifications that are provided to ensure the definition of key interoperability messages,

without ambiguity and with no flexibility to LNSP implementation.

2)  Functional specifications that provide guidance to guarantee interoperability yet provide flexibility

to the LNSP to define specific implementations.

With item 2 above requiring the LNSP to incorporate message definitions within the applicable SISICD.

This  approach  will  be further  detailed  in  future  releases  of  this  document,  including  which  category is
planned to be adopted for each of the messages.

2.5.1. LUNANET NETWORK ACCESS INFORMATION (MSG-G1)

{LSIS-TBW-2004}

2.5.2. HEALTH AND SAFETY (MSG-G2)

{LSIS-TBW-2005}

2.5.3. CLOCK AND EPHEMERIS DATA (CED, MSG-G4)

{LSIS-TBW-2006}, additional information will be provided in future versions of this document, but some
preliminary  information  is  provided  to  allow  understanding  of  the  type  of  information  expected  to  be
broadcast to the user. MSG-G4 is expected to contain or specify:

  The  data  to  allow  a  user  to  compute  the  position  and  velocity  of  the  phase  center  of  the
navigation antenna (or antenna reference point) of the LNSP node transmitting the AFS, in the
reference frame defined in LNIS AD5

  The  data  to  allow  a  user  to  correct  any  clock  synchronization  error  impacting  the  AFS

transmitted by an LNSP node, in the time scale defined in LNIS AD5

  The validity period of the data contained in MSG-G4

  The reference epoch to which the data refers

LSIS V1.0

29 January 2025

37

  The expected quality estimated by the LNSP of the data contained in MSG-G4 (e.g.: similar to

GPS URA or Galileo SISA)

2.5.4. MORBIT ALMANAC (MSG-G5)

{LSIS-TBW-2007}

2.5.5. TIME OF TRANSMISSION (MSG-G8)

The time of transmission (ToT) may be retrieved by a receiver combining multiple fields provided in the
navigation message and exploiting tracking of the spreading codes.

A user receiver may resolve the time (multiple of the frame duration) since the starting LunaNet Reference
Time (LRT) epoch with the following formula:
+

  +

=

+

Where

 is the time in seconds since the starting LRT epoch, with increments that are a multiple

of the duration of a frame.

 is the Node Time (NT) offset with respect to LRT, which will be provided as part of

MSG-G4.

The synchronization within the frame duration may be achieved counting the number of primary/secondary
codes and the phase within the primary code since the leading edge of the first chip of the first code sequence
of the frame (e.g.: the edge of the first chip of the synchronization pattern).

LSIS-720: ToT Fields Definition

The fields needed to retrieve time of transmission shall be in accordance with the following definitions (see
Figure 11):

 is the number of seconds in a week (604800).

 is the block interval duration in seconds. This is computed multiplying the frame duration

with the number of TOI values (100):

= 12 100 = 1200 seconds = 20 min.

 is the frame duration, which length is 12 seconds.

  The Time of Interval (TOI) is represented with 7 bits and represents the number of frames from
the beginning of the block interval (BI). A BI lasts 1200 seconds (20 minutes), a frame lasts 12
seconds, so TOI needs to represent 100 values. The TOI is referenced to the leading edge of the
first chip of the first code sequence of the frame (leading edge of the first chip of the SP).

Where   is the time in seconds since the LRT start epoch.

=

( ,

)

  The  Interval  Time  of  week  (ITOW)  count  is  defined  as  being  equal  to  the  number  of  block
intervals  (BI)  that  have  occurred  since  the  transition  from  the  previous  week.  The  ITOW  is
represented with 9 bits, covering the 504*BI in a week.

LSIS V1.0

29 January 2025

38

=

( ,

)

Where

is the time in seconds since the LRT start epoch.

The Week Number (WN) is an integer counter that gives the sequential week number from the
LRT start epoch. This parameter is represented with 13 bits, which covers 8192 weeks (about
157 years). Then the counter is reset to zero to cover an additional period modulo 8192.

=

Where

is the time in seconds since the LRT start epoch.

The LunaNet Reference Time (LRT) start epoch is {LSIS-TBD-2003} (LRT is defined in LNIS
AD5).

Figure 11:  Time of Transmission Concept

LSIS V1.0

29 January 2025

39

LSIS-730: ToT Fields Binary Representation
The fields required to retrieve ToT shall be according to Table 22.

Table 22 - Time of Transmission Fields Binary Representation

Parameter
TOI
ITOW
WN

Definition
Time of interval
Interval time of week
Week Number

Bits
7
9
13

Scale factor
N/A
N/A
1

Unit
dimensionless
dimensionless
week

Values
0…99
0…503
0…8191

2.5.6. MANEUVER (MSG-G10)

{LSIS-TBW-2008}

2.5.7. SATTITUDE STATE/ EPHEMERIS (MSG-G11)

{LSIS-TBW-2009}

2.5.8. CONJUNCTION (MSG-G14)

Not  a  LunaNet  1.0  item.  Placeholder  for  future  implementation  of  conjunction  messages,  which  are
intended to inform lunar users of potential conjunctions.

2.5.9. MAPLET (MSG-G15)

Not a LunaNet 1.0 item. Placeholder for future implementation of Maplet messages, which are intended to
provide localized map updates to lunar users.

2.5.10.

ANCILLARY INFO (MSG-G17)

{LSIS-TBW-2010}

2.5.11.

LUNASAR AUTOMATIC ACKNOWLEDGE (MSG-S19)

Not a LunaNet 1.0 item. Placeholder for future implementation of LunaSAR messages, intended to provide
support for lunar search and rescue activities by providing an automatic acknowledge at the LNSP node
level of receipt of a SAR beacon distress message.

2.5.12.

LUNASAR RETURN MESSAGE (MSG-S20)

LunaNet 1.0 item limited to a {LSIS-TBD-2004} reserved bit allocation for an acknowledgement of beacon
reception from a LunaSAR authority that is to be disseminated by LNSP nodes. Detailed message definition
is deferred for future implementation of LunaSAR messages, intended to provide support for lunar search
and rescue activities.

2.5.13.

ACKNOWLEDGE OF NON-SAR MSG (MSG-G22)

Not  a  LunaNet  1.0  item.  Placeholder  for  future  implementation  of  message  request  acknowledgement
messages.

LSIS V1.0

29 January 2025

40

2.5.14.

GNSS AUGMENTATION (MSG-G23)

Not  a  LunaNet  1.0  item.  Placeholder  for  future  implementation  of  GNSS  augmentation  services  (e.g.,
almanac, navigation data, etc.).

2.5.15.

DETECTION ALERT (MSG-G24)

Not  a  LunaNet  1.0  item.  Placeholder  for  future  implementation  of  alert  messages,  which  are  specific
messages to disseminate general or specific alerts to lunar users (e.g., solar storm alert, etc.).

2.5.16.

SCIENCE (MSG-G25)

Not a LunaNet 1.0 item. Placeholder for future implementation of science supporting messages.

2.5.17.

UIS RESPONSE (MSG-G27)

Not  a  LunaNet  1.0  item.  Placeholder  for  future  implementation  of  user  initiated  service  supporting
messages.

2.5.18.

USER SCHEDULE NOTICE (MSG-G28)

{LSIS-TBW-2011}

2.5.19.

FF COMMANDS (MSG-G29)

Not a LunaNet 1.0 item. Placeholder for future implementation of fast-forward command support
messages via AFS.

2.5.20.

TIME CONVERSIONS (MSG-G30)

{LSIS-TBW-2012}

2.5.21.

AUGMENTATION DIFFERENTIAL CORRECTIONS (MSG-G31)

{LSIS-TBW-2013}

2.5.22.

COORDINATE FRAME CONVERSIONS (MSG-G32)

{LSIS-TBW-2014}

LSIS V1.0

29 January 2025

41

APPENDIX A – ACRONYMS

Table A- 1 Acronyms

Acronym

Definition

t

AD

AFS

BCH

BI

BPSK

CRC

CSV

EIRP

FID

GNSS

GPS

ITOW

LANS

LDPC

LNIS

LNSP

LRT

LST

LSB

MSB

MSG

NRZ

NT

P2P

PNT

PRN

PVT

RD

RHCP

RNSS

ROI

SB1

SB2

Delta Time

Applicable Document

Augmented Forward Signal

Bose-Chaudrhuri-Hocquenghem

Block Interval

Binary Phase Shift Key

Cyclic Redundancy Check

Comma Separated Variable

Equivalent Isotropic Radiated Power

Frame Identifier

Global Navigation Satellite Systems

Global Positioning System

Interval Time of Week

Lunar Augmented Navigation Service

Low-Density Parity-Check code

LunaNet Interoperability Specification

LunaNet Service Provider

LunaNet Reference Time

LNSP System Time

Least Significant Bit

Most Significant Bit

Message

Non Return to Zero

Node Time (Time at Transmitting LNSP Node)

Point-to-Point

Position, Navigation, and Timing

Pseudo-Random Noise (code)

Position, Velocity, and Time

Reference Document

Right-Hand Circularly Polarized

Radio Navigation Satellite Service

Rollover Identifier

Subframe Block 1

Subframe Block 2

LSIS V1.0

29 January 2025

42

Acronym

SB3

SB4

Subframe Block 3

Subframe Block 4

SECWEEK

Seconds in a week

Definition

SFCG

SISA

SISE

SISEpos

SISEvel

SISICD

SP

sps

SV

TOI

ToT

URA

WN

Space Frequency Coordination Group

Signal-In-Space Accuracy

Signal-In-Space Error

Signal-In-Space Error for position

Signal-In-Space Error for velocity

Signal-In-Space Interface Control Document

Synchronization Pattern

Symbol per second

Service Volume

Time of Interval

Time of Transmission

User Ranging Accuracy

Week Number

LSIS V1.0

29 January 2025

43

APPENDIX B – SIGNAL-IN-SPACE ERROR CONTRIBUTORS

The SISE contributors for which the LNSP is responsible include the following error sources:

i.

LNSP ephemeris uncertainties or errors in the orbital products tendered to users, as represented in
the lunar reference frame. Figure B- 1 provides a representative illustration of ephemeris errors,
where point ‘A’ refers to the actual position of the AFS-providing LNSP node at a particular point
in time, and point ‘B’ represents the position information conveyed to the users via ephemeris for
the same time epoch.  A total  norm POS) value is formed from individual error contributions
from X, Y and Z components with respect to the lunar reference frame (‘R’ in the figure). This
error applies to the ephemeris portion of the navigation message, component (4) from Figure B-
1 , and includes errors due to limitations in message formatting and implementation.

Figure B- 1: Representative illustration of ephemeris errors

ii.

Position and velocity errors due to unmodeled or unreported antenna offsets with respect to the
LNSP node position and velocity provided with the navigation messages. A representative antenna
offset discrepancy is shown in Figure B- 2, where point ‘A’ represents the LNSP node’s reference
point and ‘T’ represents the transmitting element at an offset from ‘A’. In this example, a user in
the service volume ‘V’ captures measurements related with the ‘TV’ distance. If the navigation
message  only  conveys  information  for  deriving  the  ‘AV’  distance,  the  resulting  error  would
correspond to the vector defined by ‘TV’ – ‘AV’. As with item i above, this error applies to the

LSIS V1.0

29 January 2025

44

ephemeris portion of the navigation message, component (4) from Figure B- 2, and additionally
includes errors due to limitations in message formatting and implementation.

Figure B- 2: Representative illustration of antenna offset errors

iii.

iv.

LNSP timing errors due to time knowledge uncertainties, inaccurate clock correction information
conveyed to users, or unaccounted misalignments of time with the signal realization. This error
applies  to  the  clock  portion  of  the  navigation  message,  component  (4)  from Figure  B- 3,  and
includes errors due to limitations in message formatting and implementation.

Uncalibrated  or  unknown  LNSP  “group” delays  and  their  variations.  These  group  delays  and
variations  may  be  due  to  code  phase  offsets,  antenna  phase  offsets,  unaccounted  transmit  path
delays,  code-to-code  incoherency,  code-to-carrier  incoherency, etc.  As  with  item  iii above,  this
error applies to the clock portion of the navigation message, component (4) from Figure B- 3, and
includes errors due to limitations in message formatting and implementation.

Figure  B- 3 provides  a  representative  illustration  highlighting  main  contributor  components  to the
SISE of a generic LNSP node. Components numbered in the figure represent (1) the frequency and
timing subsystem (F&T), (2) the transmitting radio, (3) signal amplification, (4) navigation message,
(5) transmit paths and related electronics, (6) transmitting antenna characteristics. Note that item (4)
includes major contributions to the errors such as determining the estimates of the LNSP node’s PVT.

LSIS V1.0

29 January 2025

45

Figure B- 3: Representative illustration of main LNSP node error contributors

LSIS V1.0

29 January 2025

46

APPENDIX C – DATA CHANNEL (AFS-I) PRIMARY CODES

The data channel (I channel) spreading codes are of length 2046, comprised of 2047 length Gold codes that
are short cycled to 2046 chips.  Similar to the GPS C/A code, this sequence provides low complexity access
to users.  Since the code is modulated with randomized data symbols, its properties are further improved
symbols.
relative

randomization

effect

short

code,

data

due

the

the

the

of

to

The Gold code is composed of a preferred pair of two maximal length sequences, each of length 2
Let the generator polynomial for each sequence be denoted by
produced by
C- 1 with the resultant code denoted by ( ).

1.
( ). The shift register sequence
( ), and the resultant Gold code for the AFS primary code are shown in Figure

( ) and

( ) and

Figure C- 1: Data Channel (AFS-I) Gold code generator.

The  Gold  code  sequence  is  produced  by  initializing  the    sequence  to  [I1 I2 I3 I4 I5 I6 I7 I8 I9 I10 I11]  =
11111111111 and the  sequence to [I1 I2 I3 I4 I5 I6 I7 I8 I9 I10 I11] = 11111111111, with a delay  that is defined
the  sequence  every  2046  chips
as  a
chips).
(i.e.

function  of
short

the  PRN  number,  and

the  2047  Gold

the  natural

reinitializing

length  of

to  2046

cycling

code

I11 is the most significant bit (MSB) and I1 is the least significant bit (LSB), wherein the initialization vector
is fed into the register from LSB to MSB.

LSIS V1.0

29 January 2025

47

The  initializations considering specific delays  are provided in  - Table C- 1 through - Table C- 5 below
using the same convention as used for C/A code5. The first and last 24 chips are provided in hexadecimal
format for validation purposes.

- Table C- 1: Gold code initializations for PRNs 1-42

G2 Initialization
(HEX)
514
59E
49A
346
78B
341
17D
7AB
301
32E
4DE
29C
4ED
506
30E
230
270
38D
2DC
5C8
61E

First 24 Chips
(HEX)
5D6430
4C2FFF
6CAF44
9730AD
0E9C61
97C9DC
D05F9B
0A99BD
9FC265
9A355A
642134
AC628C
624F75
5F243A
9E3086
B9FE79
B1F5C0
8E5B4C
A46935
46E185
3C371A

Last 24 Chips
(HEX)
CF00CE
CFD228
5F87BE
A1C4FF
B34161
666CE4
71E734
31EDD1
633585
6EC903
7EA2C9
8196A2
8E2A2E
1C689D
EC65B3
650CEB
60558A
05037C
84CFC3
3D9F0C
191A6B

PRN
22
23
24
25
26
27
28
29
30
31
32
33
34
35
36
37
38
39
40
41
42

G2 Delay
(chips)
1188
390
714
303
1001
707
1984
139
182
1891
1247
1434
2000
1843
865
616
514
449
1173
24
1383

G2 Initialization
(HEX)
72C
54B
77C
32F
600
635
4B7
73D
4BE
55F
775
424
603
450
0AB
316
44F
477
76F
1F3
1B9

First 24 Chips
(HEX)
1A77BE
568A4F
106B22
9A1ECD
3FF94B
3945EC
690F54
184B0C
682F51
5418A3
114B27
7B73BB
3F85F3
75EF32
EA83E1
9D2C31
760AF4
71139F
1200BF
C1919D
C8C699

Last 24 Chips
(HEX)
FCF708
D4F9C3
685E33
CF8507
A6F603
309AFB
10E331
CC4B56
F95718
61758F
81EA1A
6875A4
45220E
5A0C39
3D3152
356DC4
44AC55
1F0892
1A7A64
5549C4
5A7081

-Table C- 2:  Gold code initializations for PRNs 43-84

G2 Initialization
(HEX)
6BE
6EB
336
650
59C
74F
294
03F
5AB
4F2
647
494
521
53D
4DF
4A2
2C3
15F
3A2
2C9
329

First 24 Chips
(HEX)
2825B4
229776
9929ED
35E5D7
4C78D0
160563
AD691E
F8067F
4A9358
61AAB3
370B83
6D7630
5BD897
5841E9
640AA3
6BB62F
A78CF3
D40D68
8BAC73
A6D04E
9ACC2B

Last 24 Chips
(HEX)
910419
809D30
B7C174
325F38
8D4A21
98D6D4
C96E8F
820FDC
59BED0
908A42
640779
719B8C
596C36
A41857
DFEECD
042379
9A6FAF
B1D38D
08FFFA
900F8B
A96118

PRN
64
65
66
67
68
69
70
71
72
73
74
75
76
77
78
79
80
81
82
83
84

G2 Delay
(chips)
1547
370
271
1353
635
299
697
152
678
1329
15
1974
1884
1868
277
302
9
603
1583
848
1234

G2 Initialization
(HEX)
214
532
733
697
6DD
2FC
471
42B
706
52F
77F
526
7FB
220
13C
65F
7E3
254
444
041
542

First 24 Chips
(HEX)
BD7E6C
59B30A
199278
2D006D
245769
A06CE9
71C179
7A8158
1F2EDF
5A01E3
10179A
5B21E6
008521
BBE95C
D87FB5
341734
039996
B575D5
777DDE
F7C64B
57AA4A

Last 24 Chips
(HEX)
C3DC4D
2B4861
E25764
FA1C80
F525C5
066373
79EC8D
E72592
743B9C
777004
8B8A3E
9EC42D
A544EA
F4FCB1
D5F251
BD0F0E
7C4C9D
C6852C
EF8075
BA1665
3D4DEA

G2 Delay
(chips)
1845
1071
170
2035
1214
1292
1284
1894
1537
735
561
1789
1453
196
1040
326
1787
982
1030
1380
1932

G2 Delay
(chips)
1940
1594
1765
752
145
1615
1666
1372
1634
1068
1181
879
1153
1621
927
1848
402
413
1090
657
609

PRN
1
2
3
4
5
6
7
8
9
10
11
12
13
14
15
16
17
18
19
20
21

PRN
43
44
45
46
47
48
49
50
51
52
53
54
55
56
57
58
59
60
61
62
63

5 NAVSTAR GPS Space Segment/ User Segment Interfaces, IS-GPS-200N, Table 3-Ib

LSIS V1.0

29 January 2025

48

PRN
85
86
87
88
89
90
91
92
93
94
95
96
97
98
99
100
101
102
103
104
105

PRN
127
128
129
130
131
132
133
134
135
136
137
138
139
140
141
142
143
144
145
146
147

G2 Delay
(chips)
1568
510
1303
1921
823
1187
1299
824
672
2034
1388
13
223
1840
1161
1132
365
2
924
1373
959

G2 Delay
(chips)
1796
100
1660
1454
1613
1064
844
518
320
661
2031
694
1143
1167
1885
833
1601
903
399
1896
899

- Table C- 3: Gold code initializations for PRNs 85-126

G2 Initialization
(HEX)
6EF
4F6
7C8
45F
654
659
486
72A
191
68D
0CD
5FE
604
282
36D
231
655
1FF
6FB
41F
6AC

First 24 Chips
(HEX)
2212BF
612F7A
06EB60
741DD1
35601E
34C5D2
6F363A
1AA558
CDC8D7
2E4BF5
E65F62
40219A
3F7C82
AFACDD
92425B
B9D5EE
354B89
C01FC6
208053
7C1668
2A65BE

Last 24 Chips
(HEX)
A4E126
B4F654
55CC0D
D55C0F
16232E
DBEB11
A2F3DF
9A1317
90241C
618CFE
DC209C
4827F9
828A15
3E7ACA
88446F
C440EF
B76F2A
39CDFF
116D6A
D0056E
426C4A

PRN
106
107
108
109
110
111
112
113
114
115
116
117
118
119
120
121
122
123
124
125
126

G2 Delay
(chips)
220
1542
188
264
453
68
715
75
1095
938
1316
394
1156
166
969
269
179
957
400
625
1513

G2 Initialization
(HEX)
026
298
612
1D0
2C7
534
7BE
7DA
51D
39A
786
354
5A4
1A4
529
4CE
5F2
2B0
30D
621
572

First 24 Chips
(HEX)
FB315F
ACE745
3DB941
C5E8F9
A7093A
5961EC
0820C6
04AB6A
5C4435
8CB518
0F39AD
9570A7
4B61BB
CB7470
5AD305
663611
41AFC1
A9E90B
9E4C3E
3BD700
51B8B3

Last 24 Chips
(HEX)
FA4BAF
A5EAB4
759E50
343179
BE13B9
4DAC7E
252D99
86A45E
26B4E7
535B3D
7E895E
72ACAC
D6EEE6
0648E4
11941B
EF5293
24A3C2
6FBE29
0FB1BE
8516B7
2E1100

- Table C- 4: Gold code initializations for PRNs 127-168

G2 Initialization
(HEX)
455
4C5
51E
676
53C
726
410
544
430
3AC
46B
389
7DE
3CD
7FD
0BB
60D
6F9
61A
7EA
798

First 24 Chips
(HEX)
75416C
67413B
5C388D
3132ED
586A7E
1B2B03
7DE48B
5778AC
79E157
8A7507
728AE1
8EDE85
042EA3
8650F5
0057C7
E894C4
3E5C87
20D77C
3CB2D3
02B993
0CF7FC

Last 24 Chips
(HEX)
DF3C2B
447EB3
C560EA
D61797
055453
F6972C
5F5558
5BA9F5
DDF9E8
26E3C8
E27CF3
217F6A
A2D848
005A1D
C3A0F5
ACC108
6B3E3C
53F563
3D667D
95F8B4
C16536

PRN
148
149
150
151
152
153
154
155
156
157
158
159
160
161
162
163
164
165
166
167
168

G2 Delay
(chips)
133
556
331
198
212
1024
1070
1972
1573
884
1177
1691
533
480
751
447
734
973
857
1767
1548

G2 Initialization
(HEX)
75A
3C7
5D1
541
6B9
71E
33C
498
377
664
476
6BB
55E
340
4A0
1DE
65D
3D2
32C
2CD
50A

First 24 Chips
(HEX)
14BC18
870C48
45D6A5
57D6F2
28DCC5
1C3268
987550
6CF86B
9109C3
3372E7
713808
288BEA
543334
97E24B
6BE100
C4318D
34401B
85B533
9A6275
A65587
5EAA61

Last 24 Chips
(HEX)
8C169C
0A3A39
45DB7F
DE99E7
56AC02
AD33EB
BDA150
1D1FB7
13D411
057FC4
BE4496
14340B
C0398B
C720E0
46BB70
1A2D4B
FF9707
1EFA71
2C510A
B4739D
70ECA6

LSIS V1.0

29 January 2025

49

- Table C- 5: Gold code initializations for PRNs 169-210

PRN
169
170
171
172
173
174
175
176
177
178
179
180
181
182
183
184
185
186
187
188
189

G2 Delay
(chips)
1876
614
1017
1978
275
1141
1252
1952
1714
1067
557
522
1159
545
1580
610
935
1134
780
691
1038

G2 Initialization
(HEX)
31A
459
750
5D2
4F3
779
53B
719
146
1E4
5E3
554
5B4
28B
226
594
4D4
48C
02C
44E
43A

First 24 Chips
(HEX)
9CA26A
74CF37
15E0A5
45AA1D
618124
10C57C
58930F
1CCB19
D73A48
C37FC9
439373
556F89
49769E
AE8CD8
BB3BBA
4D7342
657D89
6E6A87
FA6DE2
762163
78BDEA

Last 24 Chips
(HEX)
59E9FF
B3B810
8676B8
A60F72
31C646
ED6E21
C2FC48
6A9BF0
C997FE
031185
141F9C
CA59AF
471EBC
D7CEE3
9218AE
C5B20C
74C2ED
A893FB
F02B8B
E5E051
D799CC

PRN
190
191
192
193
194
195
196
197
198
199
200
201
202
203
204
205
206
207
208
209
210

G2 Delay
(chips)
1418
295
916
1654
624
706
1033
1633
790
1451
1300
459
106
861
1541
114
1381
1945
1069
242
356

G2 Initialization
(HEX)
6F0
7C5
331
7BD
443
46A
75B
357
22C
3B5
643
64B
4D3
2B2
530
76C
6E4
4B5
679
112
2DE

First 24 Chips
(HEX)
21F779
074EAC
99D09C
085C7E
7784AF
72A176
14978F
950C1F
BA6707
894227
378E4A
3685D8
6584F8
A9BE24
59E425
127C07
236595
69587B
30C00E
DDA31D
A43E1A

Last 24 Chips
(HEX)
BA414A
980432
70696F
C6F994
28286E
4330F7
2D5A98
9178A1
98788A
5EA7BB
407B6F
088342
B36AF6
2D2620
69D068
F9AE69
0FCD06
527B38
5947A1
7942D3
C657CA

LSIS V1.0

29 January 2025

50

APPENDIX D – PILOT CHANNEL (AFS-Q) PRIMARY CODES

The pilot channel (AFS-Q) primary codes use the same construction as described for the L1C Q channel
primary code6.  The pilot channel (AFS-Q) primary codes are Weil sequences of length 10230, that are
derived from a Legendre sequence L(t) of length 10223.

Each ranging code is derived from the length-10223 Legendre sequence, with a common 7-bit expansion
sequence inserted at a PRN signal number-dependent point.

Legendre Sequence

The Legendre sequence L(t), for t = 0, …, 10222. L(t) is defined as,

L(0) = 0 ;
L(t) = 1, if there exists an integer x such that t is congruent to x2 modulo 10223;
L(t) = 0, if there exists no integer x such that t is congruent to x2 modulo 10223.

The above Legendre sequence is used to construct the unique length-10223 sequence used for each ranging
code. The Legendre sequence defined in Table D- 1.

6 NAVSTAR GPS Space Segment/ User Segment L1C Interfaces, IS-GPS-800J

LSIS V1.0

29 January 2025

51

Table D- 1- AFS-Q Primary Code 10233 Legendre Sequence in HEX

3DE552ECC61973C3E2F82F06D61DE035F019FD80BB7682DB6CD2A76F7A289CEDFC82058F75
C78883BD16FCD100B9CF36794B64313A5EF97E5FB299038FE3F9CFDFEB2210A2C729DCFE6D6
A750BA328159DEEACF17D6BC42C2A2395A5637E3472FDAC6394D0C8148E1FB0EDF95F865FF
2C5D794332704089F2977783755AF4BF54FD5D13CB0980E093013407CB98F4FC95F785144F3
B0DE4C09BF92143922AC65A5C7597B114B768CD5ECFBBFC0E231EA3130989F24EF9AECD0B6F
DF21E60FE9B55EDB34AF8958EC3CC214122ACCBA3F085F5BC297BE7F506EFF5A8FA6755DE3B
C0CD4E552462BEBE10DCAAE809830F5E11AE76FE7FA23C764E6D115CE9B55CEC61D764CD64
0C3F493C23A52A83F2218C36280C3CE808DD6B37852BD441FD49A64F747FAA6424CAC3DEB5
960367506BC0232517D70E322CE015A4B21B9B4AD8E5326F6A5447ACFF148C8CC13CD6DB8B
CBE6C773C1D59797DD600B52B89E0DD9B2142494A30721A5D498E179DD0DB1DB696C291CF
157CF5E3825F870A9FFDB0514C465DBC7141E611B7D2B2E672BD16A97E36012AEE0A4B2191
B41413512B5568128CDFE6F3D6213247DB57275D6442A79F1F7FCED3301F5187854464F4FD8
B7BFC8236364C9F2E664B2EA72BB35D3B7D2ACA1CF699BB9590889A7AABC21FC45728C8D33
F254D9F5787D5DB098B74074D2E3F87946AE26E56B5187166C6F6B5B62F22FE4D6504143CCD
AE02BC0177CEB84BD42301F106633B10A17F698BA0D23C834D213A0834AB743538023A94FC
6DB36167D82ECB13FE4E8CA27DF6CEB27B8F0C3D6564AFD579B90D8E230CBEBB9621745739
467867202BE50F4642A593D408A109E90C60C774E54A3FA1E1A9B4BDA22A4451A49943A2F3
72D9C61196DA1F26C1E9259DE7192C4C2E8F59B69D776AEE90B49A9E1E80EB5634473E73DA
1BDEBBF50D96AF6743D60AFEC6786758C5745EE588A0B3CEE393D8985502B65650F3C3886C
A324106EB3A3600DCB22F9065E4C927035A8EFF8D4F44AB4FBE8D
ED34FB0ED3E8B9A405EBDC8CE67DC1FCEF50B78A3045FF0AFE29330F5F7D653602EC2E494A4
27265C79D4A5626E2A758780E2D347F44B9BC9150785419356C0CD3B3AC57701EF0AA869BB
BD958899A431EB2AD048D14C8AC6A2CB6662C1B364E4EFB0084B9034367757879D41FCCD23
0041C186AF765146C549076CDEE50C260133ADFA554ADD4DF5F49D9ECB6BE22ADFE4E05AA5
D0AC662CAD049DE61F5C709167735D7C9001ABC7816F8E143055C31DAF25A491C93D1185E3
9B5169EC7CEB5B6F5EC9913E1B8AD4BFE510585951F0C47260B0B892530DF33B3B5C0328775
6A426CD6392B4989ECB695FE32ECE3C505D6CEFF0A7D464FE594A10F2B36F66A807443669B5
01F750AD784CA513BFA30F3FAE4F39EEC0FAAD68EF0DB40F3F65336451E7231549A315DD263
6470EE806024629DE143CF9BFA2AB13DE0A0AE76D563533F08E11546683A940227D406085AF
09417BC0E8B32AEDF5EF30F2395B82B4C921549A03E61EC1024BD32298236C1B9BCDCEA1CE
E3F0088321533A44B5DC85947169672AED8F5ED809BF3613C8C375D78415B034398B07F4DFC
DBE3F9BCB0DD1503540B429544F8445AC1BBF7C6CCF585172C01678158123C81E3B5FB3D35
8E72902C74E04E56958EEAF2F70A505C32A21195FACE8BD46A5260311AC72EBDEECA0103180
E038FD9AC8160582168DCF64B5864C318BFDD3025D08FBB87144397EFB01231BAE84246AD3
2492FA448BF9019FC14FE11E527C2F82E0F0C59E7322D5610

NOTE: The above sequence is read from left to right across a row and then moves down to the next row.
Since 10224 bits are listed above to represent the length 10223 Legendre sequence, the single initial bit
value of 0 should be ignored. Thus, the first 27 values in the above sequence represented by HEX 3DE552E
are the binary bit values: 011110111100101010100101110.

LSIS V1.0

29 January 2025

52

Primary Code Weil Sequences

A set of 5111 = (10223

1)/2, length-10223 Weil sequences is then obtained from

= {

,

,

,

}

 operator represents the cyclic shift of the sequence for

where the
modulo-2  operation.
011110111100101010100101110… 0001000, a cyclic shift of the sequence by k = 4 results in
the sequence 10111100101010100101110…000100000111.

the  Legendre  sequence

  For  example,  given

 places to the left, and

in  Table  D-  1

 is the
is

This sequence, called a Weil-code, is the exclusive-or of L(t) and a cyclic shift of L(t). Equivalently, a
Weil-code W(t ; k) is specified by the Weil Index k, ranging from 1 to 5111, which represents the cyclic
shift of L(t) and is defined as,
W(t ; w) = L(t)

 L( (t + w) modulo 10223 ) for t = 0 to 10222.

Resulting Codes

Finally, the ranging code is constructed by inserting a fixed expansion sequence into the Weil code.

The expansion sequence is composed of seven-bit values, [0 1 1 0 1 0 0]. The insertion point is specified
by Insertion Index p, where p = 1 to 10223. The expansion sequence is inserted before the p-th value of the
Weil-code.

Thus, the pilot channel (AFS-Q) ranging code AFS-QP(t) is defined as,

AFS-QP(t) = Wi(t ; k), for t = 0, 1, . . . , p - 2;
AFS-QP(t) = 0, for t = p - 1;
AFS-QP(t) = 1, for t = p;
AFS-QP(t) = 1, for t = p + 1;
AFS-QP(t) = 0, for t = p + 2;
AFS-QP(t) = 1, for t = p + 3;
AFS-QP(t) = 0, for t = p + 4;
AFS-QP(t) = 0, for t = p + 5;
AFS-QP(t) = Wi(t - 7 ; k), for t = p + 6, p + 7, . . ., 10229.

For additional details in the code generation process, L1C code descriptions may serve as a reference7.

Assignments of the Weil Index, k, and insertion index p per PRN identification number, i, are given by
Table D- 2 through Table D- 6.

Note-1: The Weil and insertion indices (and the resulting hexadecimal codes) are currently {LSIS-TBC-
2020}. The codes provided in this release of the document are included to facilitate test and integration
efforts  only.  It  is  therefore  recommended  for  implementers  to  support  a  flexible  code  configuration,
enabling code changes to be applied through configuration updates.

7 NAVSTAR GPS Space Segment/ User Segment L1C Interfaces, IS-GPS-800J, Section 3.2.2.1.1

LSIS V1.0

29 January 2025

53

Table D- 2: Weil code index k and insertion index p for PRN 1-42

Weil
Index (k)
5111
5109
5108
5106
5103
5101
5100
5098
5095
5094
5093
5091
5090
5081
5080
5069
5068
5054
5044
5027
5026

Insertion
index (p)
412
161
1
303
207
4971
4496
5
4557
485
253
4676
1
66
4485
282
193
5211
729
4848
982

Initial 24
Chips
(Hex)
17D437
E0CD01
686C92
E8AA51
89A6BB
0751A2
458726
7688B3
DA38C9
AB3393
13B63E
A1D583
682D8A
1E7C22
4911E6
638CFE
F7E988
ED0495
9D2F16
386856
DA1BDC

Final 24
Chips
(Hex)
40F7E2
763E6C
00E22E
E63B1F
343BB1
6B0D78
0E7BA4
259D7D
4C4F7D
1DDAA6
35104B
EB4786
CE5EDB
9B2794
766ED2
E09D77
CBB3A3
30418B
1FD155
7CF289
05845C

PRN No.
22
23
24
25
26
27
28
29
30
31
32
33
34
35
36
37
3
39
40
41
42

Weil
Index (k)
5014
5004
4980
4915
4909
4893
4885
4832
4824
4591
3706
5092
4986
4965
4920
4917
4858
4847
4790
4770
4318

Insertion
index (p)
5955
9805
670
464
29
429
394
616
9457
4429
4771
365
9705
9489
4193
9947
824
864
347
677
6544

Initial 24
Chips
(Hex)
5F00B8
D88397
3F9E2D
9E809A
A85F8D
F8D776
4A49B8
C9B024
2F78DF
C33A66
EEAFAB
CFF4E8
6EE88D
A93C2D
D28D51
EEE25B
2B4E51
3B40B5
24E2DF
485AB0
26CC34

Table D- 3: Weil code index k and insertion index p for PRN 43-84

Weil
Index (k)
4126
3961
3790
4911
4881
4827
4795
4789
4725
4675
4539
4535
4458
4197
4096
3484
3481
3393
3175
2360
1852

Insertion
index (p)
6312
9804
278
9461
444
4839
4144
9875
197
1156
4674
10035
4504
5
9937
430
5
355
909
1622
6284

Initial 24
Chips
(Hex)
53EA04
304544
AE94B5
359E06
38D294
DE5971
9EC5E3
D45E98
FE1BF9
F5E9A9
F768D8
930082
4D1315
A689E0
9F956C
DDAA9B
C69E0D
8D1F6C
AAA032
FC77CE
B9D130

Final 24
Chips
(Hex)
3AA47B
F057C3
A912C3
EB5A67
05146A
221052
4A763B
CCCA90
D7B848
54EF1D
3EA85E
9C69F4
301C6E
27A615
FBE0D5
13A98D
EA89E3
A51A81
03313B
E51F85
A101C7

PRN No.
64
65
66
67
68
69
70
71
72
73
74
75
76
77
78
79
80
81
82
83
84

Weil
Index (k)
5065
5063
5055
5012
4981
4952
4934
4932
4786
4762
4640
4601
4563
4388
3820
3687
5052
5051
5047
5039
5015

Insertion
index (p)
9429
77
932
5973
377
10000
951
6212
686
9352
5999
9912
9620
635
4951
5453
4658
4800
59
318
571

Initial 24
Chips
(Hex)
0A4EC0
E7ABBC
5656C4
32F822
F363B4
4EFC32
86C5A8
048966
6E3822
47F935
816592
B8C5FC
8D0417
129C4D
F364F4
A87D77
9E7929
091363
4CE739
54FD88
325E9F

Final 24
Chips
(Hex)
7F749D
690298
7E3F6B
4D9164
1CD50D
0AF221
A971B4
C59E44
8CBED8
41BD37
83CBAE
A1753D
F708C1
AAD0B6
35B88C
AE4BC3
E4BC14
070F2D
EE6F11
B1F52C
26AD5C

Final 24
Chips
(Hex)
918AA6
02613D
177927
79DEB3
8B84E6
6AA22C
8A7C41
C49C84
116580
06CAB3
81FA85
81C8F3
A1F46D
6F2200
7AD22F
5FED25
AA13F6
6EF4E3
B96C3F
64C22A
89130A

PRN No.
1
2
3
4
5
6
7
8
9
10
11
12
13
14
15
16
17
18
19
20
21

PRN No.
43
44
45
46
47
48
49
50
51
52
53
54
55
56
57
58
59
60
61
62
63

LSIS V1.0

29 January 2025

54

Table D- 4:- Weil code index k and insertion index p for PRN 85-126

Weil
Index (k)
5005
4984
4975
4974
4972
4962
4913
4907
4903
4833
4778
4721
4661
4660
4655
4623
4590
4548
4461
4442
4347

Insertion
index (p)
565
9947
4654
148
3929
293
178
10142
9683
137
565
35
5949
2
5982
825
9614
9790
5613
764
660

Initial 24
Chips
(Hex)
3D58C0
3E822F
A1E801
96DBF7
008EF1
A19474
42982A
0F2FEF
0C84F1
1F3FA7
EBDF57
F397B0
9FFC9D
3413A3
E05A7D
2D8058
A7B2C4
3F3148
CD0720
31B493
F88784

Final 24
Chips
(Hex)
A5FF01
1BC1A4
37CD5B
201BB5
EE0579
FDA6C4
3567CD
2136D7
BD901C
FCC6B9
8E6A23
02F8F5
9203F6
72FCE3
67AB47
DD95C6
9B2383
D1CB09
C705E1
B1570D
8D842C

PRN No.
106
107
108
109
110
111
112
113
114
115
116
117
118
119
120
121
122
123
124
125
126

Weil
Index (k)
4259
4256
4166
4155
4109
4100
4023
3998
3979
3903
3568
5088
5050
5020
4990
4982
4966
4949
4947
4937
4935

Insertion
index (p)
4870
4950
4881
1151
9977
5122
10074
4832
77
4698
1002
5549
9606
9228
604
4678
4854
4122
9471
5026
272

Initial 24
Chips
(Hex)
7DA99E
5B06C2
0BDF45
B664A7
82FF31
3E363F
109B22
2CD9D7
FC5ECF
069715
14B535
A08941
C2A646
494DE0
29E828
6A9887
DE27B4
FD6C77
9A6311
93B2CF
81D4BF

Table D- 5: Weil code index k and insertion index p for PRN 127-168

Weil
Index (k)
4906
4901
4872
4865
4863
4818
4785
4781
4776
4775
4754
4696
4690
4658
4607
4599
4596
4530
4524
4451
4441

Insertion
index (p)
1027
317
691
509
9708
5033
9938
4314
10140
4790
9823
6093
469
1215
799
756
9994
4843
5271
9661
6255

Initial 24
Chips
(Hex)
C1B800
661930
46EB29
39B0E6
6B5435
5A986C
7133E6
FB6531
9FCFD9
09C81B
D8F696
EC1DD7
FD95F8
A74C62
B81CF3
8B0973
65D2DF
5C8CF4
8757BC
87A716
DEF5BE

Final 24
Chips
(Hex)
AB6673
896793
AC4231
D85438
10169A
DFD1B3
B34FD8
34B78C
05991C
B93196
977D8C
9C9271
AB9101
7ABCAC
0AEEE0
C671A8
B032E7
48DFEF
70C037
2F6C84
63569E

PRN No.
148
149
150
151
152
153
154
155
156
157
158
159
160
161
162
163
164
165
166
167
168

Weil
Index (k)
4396
4340
4335
4296
4267
4168
4149
4097
4061
3989
3966
3789
3775
3622
3523
3515
3492
3345
3235
3169
3157

Insertion
index (p)
5203
203
10070
30
103
5692
32
9826
76
59
6831
958
1471
10070
553
5487
55
208
645
5268
1873

Initial 24
Chips
(Hex)
2D22CF
02CC3F
580291
542794
18F1B5
BB9D25
88FC1D
B37536
601073
2A612C
0A3699
9165AD
F26D0F
82B997
FE8EC1
FC4FE1
1BF406
827650
718184
D48F0F
E20051

Final 24
Chips
(Hex)
0E57E1
A9362E
8CB07D
29422C
F5EA9B
463A4E
8370F5
D16AB3
60782F
F4A368
B3C9A1
159422
8C8769
A5F559
88BB01
60F3FD
225B5D
05A897
6769B1
14049F
6302B3

Final 24
Chips
(Hex)
594679
3A17B4
B5EC1D
4A3404
2CA716
AACFA5
B94640
803B9B
4C938B
95A80E
8D6C79
6F7479
940F18
9CE474
320797
1D4941
D2CBB6
29F527
2C7CFE
81EF8C
F21CDB

PRN No.
85
86
87
88
89
90
91
92
93
94
95
96
97
98
99
100
101
102
103
104
105

PRN No.
127
128
129
130
131
132
133
134
135
136
137
138
139
140
141
142
143
144
145
146
147

LSIS V1.0

29 January 2025

55

Table D- 6: Weil code index k and insertion index p for PRN 169-210

PRN No.
169
170
171
172
173
174
175
176
177
178
179
180
181
182
183
184
185
186
187
188
189

Weil
Index (k)
3082
3072
3032
3030
4582
4595
4068
4871
4514
4439
4122
4948
4774
3923
3411
4745
4195
4897
3047
4185
4354

Insertion
index (p)
427
367
1404
5652
5
368
451
9595
1030
1324
692
9819
4520
9911
278
642
6330
5508
1872
5445
10131

Initial 24
Chips
(Hex)
55149D
F30112
283513
2F3548
E6902D
F4C698
96A1C9
E55A63
F23582
D28563
C948AF
B899CC
42CBFA
7241D1
5C97C1
B59B3B
0C041D
4A179C
A0CFB1
7B9756
DD5A38

Final 24
Chips
(Hex)
05F033
9B5C39
66A6F1
3FAA28
C2E065
E3E46B
CF9B9D
6DDC00
C14175
7ED633
BC2936
B92953
E765D3
AA83CB
6A8D47
A50B05
2FEA11
571508
5DFFEF
46D6BF
444851

PRN No.
190
191
192
193
194
195
196
197
198
199
200
201
202
203
204
205
206
207
208
209
210

Weil
Index (k)
5077
4042
2111
4311
5024
4352
4678
5034
5085
3646
4868
3668
4211
2883
2850
2815
2542
2492
2376
2036
1920

Insertion
index (p)
422
4918
787
9864
9753
9859
328
1
4733
164
135
174
132
538
176
198
595
574
321
596
491

Initial 24
Chips
(Hex)
3D91CD
4379C6
47E900
E370A8
53BEFB
126E82
0AD2C0
695566
20A2D9
22BF9C
6818BD
2CAC1F
C38171
F5DD38
42C78B
6FEE31
17B83C
797E33
10E6A3
E6469F
589648

Final 24
Chips
(Hex)
E63108
060C9C
216959
2741E6
A76283
371180
E09E7C
FF1AB1
4A4E56
D42522
854752
BB0B5E
2C21DE
370B59
09F308
208FA2
209BB5
EB138C
B8D1AD
B060BB
2B48AC

LSIS V1.0

29 January 2025

56

APPENDIX E – PILOT CHANNEL (AFS-Q) TERTIARY CODES

Similar to AFS-Q primary code construction, the 1500 symbol tertiary code used for the pilot channel (AFS-
Q) tertiary code is constructed as a Weil code from a Legendre sequence.

Legendre Sequence

The Legendre sequence for the pilot channel (AFS-Q) tertiary code is of 1499 length and is defined in
Table E- 1.

Table E- 1: AFS-Q Tertiary Code Length-1499 Legendre Sequence in HEX

2E27C52A95BB664C711AE8C3278736BFA3F97ACEACF435494D027D81EBA6EA2A4F6
200D928C49EA44EBC091DC3B936F33C99DFEDA818FF7086606F868E6D6E6D9E292F4
77D56BA79447C3D1650ACEFBF16844237DE7BFA9A97C24AF1E1AE29E1C2B6F05A9A
8086104EF77A5C08232BD65D0F0775868A5504742DAE192625263A7827E67BC4039FA9
20119B0CC24D88F11DBF0A376A1B73AD93FEE436AEA268A1F906FD35B54F432A3285
80E80A4C786CF3A29DC73664895AAD706E2
The binary “0” of the first hex symbol “2” should be removed when converting the Legendre
sequence to binary bits.  Thus, for the first symbol, 2=010 instead of 0010. The remainder of the
symbols represent 4 binary bits each, totaling to 1499 bits.

Tertiary Code Weil Sequences

The set of 749=(1499-1)/2, length-1499 Weil sequences are then obtained as
= {

}

,

,

,

where the
the modulo-2 operation.

 operator represents the cyclic shift of the sequence for

 places to the left, and

 is

This sequence, called a Weil-code, is the exclusive-or of the Legendre sequence L(t) and a cyclic shift of
L(t). Equivalently, a Weil-code W(t ; k) is specified by the Weil Index k, ranging from 1 to 749, which
represents the cyclic shift of L(t) and is defined as, W(t ; w) = L(t)
 L((t + w) modulo 1499) for t = 0 to
1498.

Resulting Codes

To obtain the 1500 symbol tertiary sequence, a “0” is appended to the end of the sequence.  This process is
shown schematically in Figure E- 1.

LSIS V1.0

29 January 2025

57

Figure E- 1: Tertiary Weil code generation process.

Assignment of Weil Index, k, per PRN identification number, i, are given by Table E- 2 through
Table E- 6.

Table E- 2: 1500 Chip tertiary Weil code index k for PRN 1-42

Weil
Index
(k)
1
229
237
241
253
254
255
256
257
267
276
283
301
319
327
328
333
334
335
339
340

Last 24
First 24
Chips
Chips
(HEX)
(HEX)
F2164C
E4D09E
90CD30
B5F500
6EF816
E6C519
A150F2
F4E6B7
CF970A 7D78D0
7BFE8A
08E7EE
132D8A E3D990
C28B8B 35A56C
995C96
61C789
EB4428
7C4218
3DD544
476A92
424DDE
CEC3C3
C299A6
7BE699
12F383
27308C
934558
E04697
245DB1 D49CFE
5E08FA FC2A9A
0A4378
58C16B
E690BC
555249
CD93B1
27DA44
7FF7FD BDA2C6

PRN
22
23
24
25
26
27
28
29
30
31
32
33
34
35
36
37
38
39
40
41
42

Weil
Index
(k)
346
347
350
354
356
357
361
364
365
366
368
373
378
381
382
383
384
386
387
389
390

Last 24
First 24
Chips
Chips
(HEX)
(HEX)
45CD54
B25243
798CE6
807419
BD9311
1204D8
41863D 6E9C18
2B6954 AC4AB4
B20236 AA8324
B89446
E6E3D8
7A91ED E97D2A
20EC18
11F345
C73614
B3CE7C
31A9F3 D90320
4FD15E
E080AC
C5AB44
959EAA
9369FD 7296BC
173B36
C20365
60D655 DC6022
4AD608
257C35
3D62F6
B88175
88D3A2
95D274
35745A
7A3871
98FEF8
10A07C

PRN
1
2
3
4
5
6
7
8
9
10
11
12
13
14
15
16
17
18
19
20
21

LSIS V1.0

29 January 2025

58

Table E- 3: 1500 Chip tertiary Weil code index k for PRN 43-84

Weil
Index
(k)
394
397
398
400
401
407
408
414
415
416
426
431
432
433
436
437
438
439
440
441
447

Last 24
First 24
Chips
Chips
(HEX)
(HEX)
92B0E7  C13E0C
2BB4E0  D79388
B3B95E  5D315C
E394DA  62FFA2
23F92A  37E90A
B1E792  D73E7A
871FBB  5C6ABA
8843F5  37D228
F45775  9DB21E
0C7E74  C97272
9BB40E  50D744
A33F0C  755DF4
A2AE86  18ADA6
A18D93  C34D02
B25F46  C40BFA
806E12  7A01BA
E40CBA  06153A
2CC9EA
FE3C3A
BD434A  0E6E38
9E560B  EECA3E
DA2FE5  9FF324

PRN
64
65
66
67
68
69
70
71
72
73
74
75
76
77
78
79
80
81
82
83
84

Weil
Index
(k)
448
449
450
451
459
460
461
463
467
468
469
471
474
475
477
480
481
485
487
488
489

Last 24
First 24
Chips
Chips
(HEX)
(HEX)
508F55  CDF006
45CE34
69F640
6F4CF6  21FACC
3A4972  B1E3D4
5AB7E2  401D08
51BF5B  722C5C
47AE29  164EF4
4F0104
33C904
A4276C  BEC1C8
AC9E47  8F95DE
BDEC11  ED3DF2
DAC1E7  A2CD1A
683CE1  C80B3C
34A95C  620034
FFD4D1  9E3A06
409556  2FB3DE
65FA33  AD71F0
C71411  99CE82
3121E7  7100D8
869351
1017FC
E9F63C  D239B6

PRN
43
44
45
46
47
48
49
50
51
52
53
54
55
56
57
58
59
60
61
62
63

LSIS V1.0

29 January 2025

59

Table E- 4:- 1500 Chip tertiary Weil code index k for PRN 85-126

Weil
Index
(k)
490
491
492
495
496
498
500
502
507
509
510
513
515
517
520
521
524
525
526
527
528

Last 24
Chips
(HEX)

First 24
Chips
(HEX)
PRN
373CE6  566522  106
8AA953  5EDC08  107
F18239  4FAE5E  108
322214  A3111E  109
8094B6  B43470  110
2F237B  C6EB12  111
91FC4F  0D969E  112
6A809E  2060AA  113
85AD18  63A008  114
3BC5C1  98BAF2  115
935B1D  C363A8  116
24EB37  C57EA8  117
BEDD7E  03C076  118
D6045B  193B0A  119
0E1104  17BBBC  120
F8F297  DD6134  121
79A765  356A4E  122
179E55  98C2D0  123
CBEC34  C393EC  124
7308F7
753196  125
02C170  187560  126

Weil
Index
(k)
529
531
533
538
540
542
543
544
549
551
552
553
554
555
556
557
562
568
570
573
574

Last 24
First 24
Chips
Chips
(HEX)
(HEX)
E1527F  C2FC8C
A8385F  1DC8E0
8D90DF  611952
67A53F  4C9F30
B3E55D  244614
E2E4D7  872282
211930
FC534A
A6E2FE  0AB0D8
09E114  39AE7A
0AF5F3
F0833E
F13B78  131030
06A66F  D4362E
E99C40  5A7A10
37E81E
46E26E
8B00A2  7FD290
F2D1DB  0DB36E
8F85A9  D9D8AE
AEC772  5B572C
966C6B  7B6662
0D5085  0550F8
F8B7BC
FE7194

PRN
85
86
87
88
89
90
91
92
93
94
95
96
97
98
99
100
101
102
103
104
105

LSIS V1.0

29 January 2025

60

Table E- 5- 1500 Chip tertiary Weil code index k for PRN 127-168

Weil
Index
(k)
575
576
577
578
579
580
582
586
591
592
594
595
596
597
598
599
601
604
605
606
607

PRN
127
128
129
130
131
132
133
134
135
136
137
138
139
140
141
142
143
144
145
146
147

9394

99982

Last 24
Chips
(HEX)
37936
F4E420

First 24
Chips
PRN
(HEX)
148
1833B7
D4B7F0
149
4DBF7E  1BDE0E  150
7FAE62  C5AA50  151
1B8C5B  7942EC  152
D3C828
153
625101  147480  154
155
BDA738
6159DA  5C853C  156
26632B  4B1C34  157
B4FD0F  3A4A06  158
8D2A80  868242  159
FE859F
FF12CA  160
19DBA1  0C33DA  161
D767DC  EA71F8  162
4A1F26  26F5BE
163
050D39  8DEC2C  164
965A17  B10280  165
C864B1  90134E  166
7419FD  D230D2  167
0CE365  A14850  168

Weil
Index
(k)
608
609
610
612
616
617
618
619
621
622
627
628
631
633
635
638
639
640
645
647
648

Last 24
First 24
Chips
Chips
(HEX)
(HEX)
5EF99C
FD1655
1EFC34
4FE576
D928F7  6DDCA0
49D27D  A14850
5A5486
0590F4
46BF40
EFF176
3B3272
7F68CE
92B47B  0CC7D0
67A04F
252592
2B9001  B85D68
A7BEE2
641858
ABAD5A  3A26FE
E3590E
0F541A
A0159B  2B6AB8
AD27CE  BB9032
D70DAB  02E27A
F7D2BA
4ACBC8
71470E
1DB338
FD5F07  D9D24E
D80DBD  7173E8
10F19E
54CBE5

LSIS V1.0

29 January 2025

61

Table E- 6: - 1500 Chip tertiary Weil code index k for PRN 169-210

Weil
Index
(k)
649
650
651
654
656
658
660
661
662
665
668
669
671
674
675
676
678
680
682
683
684

PRN
169
170
171
172
173
174
175
176
177
178
179
180
181
182
183
184
185
186
187
188
189

59EF14

Last 24
Chips
(HEX)

First 24
Chips
(HEX)
PRN
4D4755  D3F570  190
7E5E34  55FCAC  191
186CF7
192
7D5465  111B40  193
D82036  5257D4  194
4DF179  5F6580
195
1AB445  6BACD4  196
197
D1B814  254FE4
47A0B7  B88986  198
833665  1A2FD4  199
A780F5  0F1D48  200
ABD174  EC2CDE  201
823470  A689AA  202
AF905E  EA2EB8  203
BBF023  264B3E  204
9330D9  BE8032  205
61B2C7  EC3A1A  206
ABBABF  A6D2BC  207
839B5E  8D7022  208
E3E623
E8F60A  209
231CD8  23FA5A  210

Weil
Index
(k)
686
687
688
689
690
691
697
698
702
705
707
710
716
717
718
720
722
723
725
726
729

Last 24
First 24
Chips
Chips
(HEX)
(HEX)
A102C1  99D3BE
A6D51D  C1B132
A97AA5  71742A
B625D5  10FE1A
889B34  D3EA7A
F5E6F6
55C2BA
361083  5DD210
49B26C
88F198
17AEAF  D5F75E
0346A1  71D910
206B25  D15E96
BD6AF2  549752
1511B6  08A834
CEF3F2
E34624
79377A  349A06
C9AC49  C452CC
0BC087  0771E0
F35190
FCF58C
E5ECE0
E037E1
24BF5D  39CF8E
101F92
9BC932

LSIS V1.0

29 January 2025

62

APPENDIX F – CODE ASSIGNMENTS PER LNSP

Each LNSP node identifier is assigned a unique combination of code sequences as defined in  Table F- 1.

Note:  The  assignment  between the  LNSP  node  identifiers  and combination of  code  sequences  are
currently  {LSIS-TBD-2001},  and  codes  provided  in  this  release  of  the  document  are  included  to
facilitate  test  and  integration  efforts  only.  The  assignment  of  codes  to  a  particular  LNSP  node
identifier will be completed in a future release, to allow for optimization of the assignments between
primary, secondary, and tertiary codes as well as the relevant phasing of the tertiary code.

Primary codes for AFS-I are provided above in Appendix C .  Primary, secondary and tertiary codes for
AFS-Q  are  provided  in  Appendix  D  and  Appendix  E  ,  respectively.  Note  that  the  first  12  code
assignments provided in Section 2.3.5.4 in this release are for interim use for test and integration purposes
only.

Table F- 1: - Code Assignments per LNSP node Identifier {LSIS-TBD-2001}

LNSP
Node
Identifier

Data Channel
(AFS-I)

Primary PRN

1
2
3
4
5
6
7
8
9
10
11
12
13
14
15
16
17
18
19
20
21
22

TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD

Pilot Channel
(AFS-Q)

Secondary
Code
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD

Tertiary
PRN
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD

Primary
PRN
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD

Tertiary PRN
Phase Offset
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD

LSIS V1.0

29 January 2025

63

LNSP
Node
Identifier

Data Channel
(AFS-I)

Primary PRN

23
24
25
26
27
28
29
30
31
32
33
34
35
36
37
38
39
40
41
42
43
44
45
46
47
48
49
50
51
52
53
54
55
56
57
58
59
60
61

TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD

Pilot Channel
(AFS-Q)

Secondary
Code
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD

Tertiary
PRN
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD

Tertiary PRN
Phase Offset
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD

Primary
PRN
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD

LSIS V1.0

29 January 2025

64

LNSP
Node
Identifier

Data Channel
(AFS-I)

Primary PRN

62
63
64
65
66
67
68
69
70
71
72
73
74
75
76
77
78
79
80
81
82
83
84
85
86
87
88
89
90
91
92
93
94
95
96
97
98
99
100

TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD

Pilot Channel
(AFS-Q)

Secondary
Code
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD

Tertiary
PRN
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD

Tertiary PRN
Phase Offset
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD

Primary
PRN
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD

LSIS V1.0

29 January 2025

65

LNSP
Node
Identifier

Data Channel
(AFS-I)

Primary PRN

101
102
103
104
105
106
107
108
109
110
111
112
113
114
115
116
117
118
119
120
121
122
123
124
125
126
127
128
129
130
131
132
133
134
135
136
137
138
139

TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD

Pilot Channel
(AFS-Q)

Secondary
Code
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD

Tertiary
PRN
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD

Tertiary PRN
Phase Offset
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD

Primary
PRN
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD

LSIS V1.0

29 January 2025

66

LNSP
Node
Identifier

Data Channel
(AFS-I)

Primary PRN

140
141
142
143
144
145
146
147
148
149
150
151
152
153
154
155
156
157
158
159
160
161
162
163
164
165
166
167
168
169
170
171
172
173
174
175
176
177
178

TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD

Pilot Channel
(AFS-Q)

Secondary
Code
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD

Tertiary
PRN
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD

Tertiary PRN
Phase Offset
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD

Primary
PRN
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD

LSIS V1.0

29 January 2025

67

LNSP
Node
Identifier

Data Channel
(AFS-I)

Primary PRN

179
180
181
182
183
184
185
186
187
188
189
190
191
192
193
194
195
196
197
198
199
200
201
202
203
204
205
206
207
208
209
210

TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD

Pilot Channel
(AFS-Q)

Secondary
Code
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD

Tertiary
PRN
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD

Tertiary PRN
Phase Offset
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD

Primary
PRN
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD
TBD

LSIS V1.0

29 January 2025

68

APPENDIX G – LIST OF ALGORITHMS

{LSIS-TBW-2015}

LSIS V1.0

29 January 2025

69

APPENDIX H – CYCLIC REDUNDANCY CHECK DEFINITION

This CRC definition applies to Frame ID = 0 (FID0) as defined in Section 2.4.3.1.

Twenty-four bits of CRC provide protection against burst as well as random errors with a probability of

-24 = 5.96×10-8

in the forward direction  on a  given  message using a seed of 0. The  sequence  of 24 bits (p1,p2,...,p24)  is
generated from the sequence of information bits (m1,m2,...,mk) (MSB to LSB sequence) in a given message.
This is done by means of a code that is generated by the polynomial

( ) =

where

=

1 for  = 0, 1, 3, 4, 5, 6, 7, 10, 11, 14, 17, 18, 23, 34
0 otherwise

This code is called CRC-24Q. The generator polynomial of this code is in the following form (using binary
polynomial algebra):

( ) = (1 + ) ( )

where p(X) is the primitive and irreducible polynomial

( ) =

+

+

+

+

+

+

+

+

+

+ 1

When, by the application of binary polynomial algebra, the above g(X) is divided into m(X)X24, where the
information sequence m(X) is expressed as

( ) =

+

+

+ +

The result is a quotient and a remainder R(X) of degree < 24. The bit sequence formed by this remainder
represents the CRC sequence. CRC bit pi, for any i from 1 to 24, is the coefficient of X24-i in R(X).
This code has the following characteristics:

It detects all single bit errors per code word.
It detects all double bit error combinations in a codeword because the generator polynomial g(X)
has a factor of at least three terms.
It detects any odd number of errors because g(X) contains a factor 1+X.

It detects most large error bursts with length greater than the CRC length r = 24 bits. The fraction
of error bursts of length b > 24 that are undetected is:

2-24 = 5.96 × 10-8, if b > 25 bits
2-23 = 1.19 × 10-7, if b = 25 bits

LSIS V1.0

29 January 2025

70

APPENDIX I – REFERENCE AFS LINK BUDGET

{LSIS-TBW-2016}

LSIS V1.0

29 January 2025

71

APPENDIX ZZ– TABLE OF TBXS

Table ZZ- 1 below lists specific to be determined (TBD), to be confirmed (TBC) and to be written (TBW)
items in this document (AD1 Vol-A). These items are to be confirmed or currently undefined at the release
of this version.

Each designator is numbered based on the document title, document version number when TBD/TBR was
identified, parent section number, and number of the particular unresolved item. For example, “LSIS-TBD-
2004”  would  be  interpreted  as  the  4th  unresolved  TBD  item  identified  in  LunaNet  Signal-In-Space
Recommended Standard Section 2. Once each item is dispositioned, the resolution will be substituted in
place  of  the  designator  and  the  item  will  be  struckthrough  {LSIS-TBD-2004}  in  Table  ZZ-  1.  If  new
unresolved items are identified, they will be added to this table using the above defined designation scheme.
All TBD/TBR will retain their original numbers and will not be renumbered as items are added or deleted.

Designation

Section /
Specification
ID
1 / N/A

LSIS-TBD-
1001
LSIS-TBC-2001  2.2.3 / LSIS-

Table ZZ- 1: Table of TBXs

Title

Description

SISICD
Distribution
SISE Pos.

Distribution to which LNSP should provide the
SISICD
SISE Position Error specification

001

LSIS-TBC-2002  2.2.3 / LSIS-

001

LSIS-TBC-2003  2.2.3 / LSIS-

SISE Pos.
period
SISE Vel.

SISE Position Error period of performance

SISE Velocity error specification

LSIS-TBC-2004  2.2.3 / LSIS-

002

LSIS-TBW-
2001

002
2.2.3 / LSIS-
0006

LSIS-TBW-
2002
LSIS-TBC-2005  2.3.1.2 /

2.2.3 / LSIS-
007

LSIS-050

LSIS-TBC-2006  2.3.1.3 /

LSIS-060

LSIS-TBC-2007  2.3.1.4 /

LSIS-TBW-
2003
LSIS-TBC-2008  2.3.1.5 /

LSIS-070
2.3.1.4 /
LSIS-080

LSIS-090

LSIS-TBC-2009  2.3.1.5 /

LSIS-100

SISE Velocity error period of performance

Specification for SISE position prediction
messaging

Specification for SISE velocity prediction
messaging
Definition of RHCP axial ratio value

SISE Vel.
period
SISE Pos.
prediction
accuracy
SISE Vel.
prediction
RHCP axial
ratio
AFS Phase
Specified value for carrier tracking error due to
Noise
AFS transmitter phase noise performance.
Max in-band
Maximum In-Band spurious emissions
spurious
specification
Max out-of-
Specification for maximum out-of-band
band emissions
emissions for AFS transmitters
AFS reference
AFS reference bandwidth specification used
towards evaluating correlation losses
bandwidth
Correlation loss  Correlation loss specification due to

transmitter payload distortions

LSIS V1.0

29 January 2025

72

Designation

Section /
Specification
ID
LSIS-TBC-2010  2.3.1.6 /

LSIS-103

LSIS-TBC-2011  2.3.1.6 /

LSIS-103

LSIS-TBC-2012  2.3.1.7 /

LSIS-110

LSIS-TBC-2013  2.3.1.7 /

LSIS-110

LSIS-TBC-2014  2.3.1.8 /

LSIS-120

LSIS-TBC-2015  2.3.1.8 /

LSIS-120

LSIS-TBC-2016  2.3.4 / LSIS-

160

LSIS-TBC-2017  2.3.4 / LSIS-

LSIS-TBC-
2017a
LSIS-TBC-2018  2.3.4 / LSIS-

170
2.3.4 / LSIS-
171

180

LSIS-TBC-2019  2.3.4 / LSIS-

190

LSIS-TBC-2020  2.3.5.3.2/
LSIS-222

LSIS-TBD-
2001

2.3.5.4 /
LSIS-260

LSIS-TBC-2021  2.4.1.2 /

LSIS-330
LSIS-TBC-2022  2.4.3.1.6 /

LSIS-FID0-
550

LSIS-TBC-2023  2.4.3.1.7 /

LSIS-FID0-
560

LSIS-TBC-2024  2.4.3.1.8 /

LSIS-FID0-
600

Title

Description

AFS-I power
percentage
tolerance
AFS-Q power
percentage
tolerance
Min AFS
received power
within Service
Volume (SV)
Max AFS
received power
within SV
Max AFS
received power
outside of SV
Applicability of
LSIS-120
Primary code-
data coherency
Pilot code-code
coherency
Pilot code-code
coherency
Data to pilot
code-code
coherency
Code-carrier
coherency
 (AFS-Q)
Primary Code
Definition
Code
assignments per
LNSP node
identifier
Synch pattern

Subframe 2
spare bits

Subframe 3 bit
allocation

Subframe 4 bit
allocation

Tolerance value for the AFS-I power
percentage allocation

Tolerance value for the AFS-Q power
percentage allocation

Minimum received power level for AFS-I
(data channel) within the Service Volume

Maximum received power level for AFS
within the Service Volume

Maximum received power level for AFS
outside the Service Volume

Applicability of maximum power outside of
LNSP SV specification
Primary code-to-data coherency value in
nanoseconds
Code-to-code coherency value in nanoseconds
for the primary to secondary pilot codes
Code-to-code coherency value in nanoseconds
for the secondary to tertiary pilot codes
Code-to-code coherency value in nanoseconds
for the data code to pilot code

Code-to-carrier coherency value in
nanoseconds for both data and pilot channels
Confirmation of AFS-Q (pilot) spreading
codes.

Code assignments per LNSP node identifier
shown in Table F- 1

Specification of the bit sequence for the
synchronization pattern
Quantity of mandatory spare bits to be kept for
future use in SB2

Allocations of bits to each field of SB3

Allocations of bits to each field of SB4

LSIS V1.0

29 January 2025

73

Designation

LSIS-TBD-
2002

LSIS-TBW-
2004
LSIS-TBW-
2005
LSIS-TBW-
2006
LSIS-TBW-
2007
LSIS-TBD-
2003
LSIS-TBW-
2008
LSIS-TBW-
2009
LSIS-TBW-
2010
LSIS-TBD-
2004
LSIS-TBW-
2011
LSIS-TBW-
2012
LSIS-TBW-
2013
LSIS-TBW-
2014
LSIS-TBW-
2015
LSIS-TBW-
2016

Section /
Specification
ID

2.4.3.1.9 /
Table 21

2.5.1

2.5.2

2.5.3

2.5.4

2.5.5 / LSIS-
720
2.5.6

2.5.7

2.5.10

2.5.12

2.5.18

2.5.20

2.5.21

2.5.22

Title

Description

MSG-G3
subframe
allocation
MSG-G1
specification
MSG-G2
specification
MSG-G4
specification
MSG-G5
specification
LRT start epoch
definition
MSG-G10
specification
MSG-G11
specification
MSG-G17
specification
MSG-S20
specification
MSG-G28
specification
MSG-G30
specification
MSG-G31
specification
MSG-G32
specification

MAntennaProperties LNIS message subframe
allocation

LunaNet Network Access Information
message specification
Health and Safety message specification

Clock and Ephemeris Data message
specification
MOrbit Almanac message specification

LunaNet Reference Time start epoch definition
in LNIS AD5
Maneuver message specification

SAttitude State/Ephemeris message
specification
Ancillary info message specification

Specification of number of bits allocated to
MSG-S2
User Schedule Notice message specification

Time Conversion message specification

Augmentation Differential Corrections
message specification
Coordinate Frame Conversions message
specification

Appendix G   User algorithms  Example algorithms for user implementation

Appendix I   Reference AFS

Reference link budget for the AFS signal

Link Budget

LSIS V1.0

29 January 2025

74

