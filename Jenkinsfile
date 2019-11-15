
Jenkinsfile (Declarative Pipeline)

pipeline {
    agent { docker { image 'golang' } }
    stages {
        stage('build') {
            steps {
                sh 'clang --version'
            }
        }
    }
}

