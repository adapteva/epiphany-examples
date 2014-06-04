#! /usr/bin/env bash

# This script installs the epiphany_tests project in Jenkins
# so that the Epiphany Tests will be run nightly after the
# Epiphany build

jenkins_home=/var/lib/jenkins

if [ ! -d $jenkins_home ]; then
	printf "Jenkins is not installed! Aborting..."
	exit 1
fi

sudo tar xz -C ${jenkins_home}/jobs/ -f ./epiphany_tests_jenkins.tar.gz

sudo chown -R jenkins:jenkins ${jenkins_home}/jobs

sudo service jenkins restart

