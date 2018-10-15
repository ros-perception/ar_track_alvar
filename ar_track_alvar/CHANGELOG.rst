^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package ar_track_alvar
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.7.1 (2017-06-14)
------------------
* [maintenance] Remove unnecessary metapkg.
* [fix][build][CMakeLists] Prevent rosdep errors that only occur when running tests with catkin_make_isolated. See http://answers.ros.org/question/262558/buildfarm-missing-package-dependencies-pkg_namepackagexml/
* [test] Relax tf test criteria `#39 <https://github.com/ros-perception/ar_track_alvar/pull/39>`_
* Contributors: Isaac I.Y. Saito

0.7.0 (2017-04-21)
------------------
* Consolidate ar_track_alvar* packages into a single repo (`#120 <https://github.com/sniekum/ar_track_alvar/issues/120>`_)
* Contributors: Isaac I.Y. Saito

0.6.3 (2017-02-09)
------------------
* [fix] Marker no longer recognized, for IndividualMarkersNoKinect `#93 <https://github.com/sniekum/ar_track_alvar/issues/93>`_
* [capability] Add param to derive camera frame from pointcloud message frame (`#111 <https://github.com/sniekum/ar_track_alvar/issues/111>`_)
* [capability ] individual marker nodes: replace command line args with ros parameters (`#99 <https://github.com/sniekum/ar_track_alvar/issues/99>`_)
* [maintenance] Add system test using .bag. (`#106 <https://github.com/sniekum/ar_track_alvar/issues/106>`_)
* Contributors: Hans-Joachim Krauch, Isaac I.Y. Saito

0.6.2 (2017-02-07)
------------------
* [fix] Marker no longer recognized `#93 <https://github.com/sniekum/ar_track_alvar/issues/93>`_
* [fix] add install rule for bundles folder; fixes `#88 <https://github.com/sniekum/ar_track_alvar/issues/88>`_
* [fix] Shutdown camera info sub after called
* [enhancement] add mark_resolution and mark_marge as input option
* [enhancement] individualMarkers: replace cout with ROS_DEBUG_STREAM (`#101 <https://github.com/sniekum/ar_track_alvar/issues/101>`_)
* Contributors: Alex Reimann, Hans-Joachim Krauch, Isaac I.Y. Saito, TORK Developer 534

0.6.1 (2016-06-08)
------------------
* New parameter -array to create an array of markers `#85 <https://github.com/sniekum/ar_track_alvar/issues/85>`_ from 130s/kinetic/add_60
* Fix build for Kinetic by adding missing dependencies on gencfg `#84 <https://github.com/sniekum/ar_track_alvar/issues/84>`_ from 130s/kinetic/fix_buildfarm
  
* [sys] Add a maintainer to receive notification from ros buildfarm.
* Contributors: Jackie Kay, Mehdi, Isaac I.Y. Saito

0.6.0 (2016-06-01)
------------------
* Made compatible to ROS-Kinetic-Kame `#80 <https://github.com/sniekum/ar_track_alvar/issues/80>`_
* [Travis CI] Add ROS Kinetic support. Add Prerelease Test on Travis `#79 <https://github.com/sniekum/ar_track_alvar/issues/79>`_
* Contributors: Sepehr MohaimenianPour, Isaac I.Y. Saito

0.5.3 (2016-02-02)
------------------
* [feat] New bool-Topic to enable/disable the marker detection (`#70 <https://github.com/sniekum/ar_track_alvar/issues/70>`_)
* [feat] added public way to set intrinsicCalibration for Camera class
* [fix] not publishing marker that are facing in the same direction as the camera.
* [sys] removed duplicate code for image subscription
* Contributors: Nikolas Engelhard, Scott Niekum

0.5.2 (2015-11-27)
------------------
* [fix] Move tf include from header to cpp files, fixes `#66 <https://github.com/sniekum/ar_track_alvar/issues/66>`_
  The header currently prevents us from re-using the library as a given library (because it pulls in tf2 which causes trouble). The include has been moved to the individual nodes which actually use a TransformBroadcaster.
* [fix] proper virtual destruction `#63 <https://github.com/sniekum/ar_track_alvar/issues/63>`_.
* improve license information in package.xml (`#58 <https://github.com/sniekum/ar_track_alvar/issues/58>`_)
* Added time stamp to header (`#57 <https://github.com/sniekum/ar_track_alvar/issues/57>`_)
  Previously, each pose had a timestamp, but the whole message did not. By including the timestamp for the whole message, it is now possible to use the results of the ar_pose_marker topic with other messages using message_filters::Synchronizer.
* Contributors: Alex Henning, Bener Suay, Lukas Bulwahn, Scott Niekum, Tim Niemueller, Isaac I. Y. Saito

0.5.1 (2015-04-14)
------------------
* Remove meta pkg; ar_track_alvar is 'unary stack' so no need for the meta pkg.
* Contributors: Scott Niekum, Isaac IY Saito

0.5.0 (2014-06-25)
------------------
* move README to root directory
* Merge remote-tracking branch 'origin/hydro-devel' into indigo-devel
* ar_track_alvar package uses ar_track_alvar_msgs
* restructuring packages. Separate out the message package.
* Contributors: Jihoon Lee

0.4.1 (2013-11-28)
------------------

0.3.3 (2013-02-22)
------------------

0.3.2 (2013-02-18)
------------------

0.3.1 (2013-02-14)
------------------

0.3.0 (2013-01-17)
------------------

0.2.0 (2012-08-08)
------------------
