workspace "Medical Record System" "Система управлением медицинскими записями" {

    !identifiers hierarchical

    model {
        User = person "User" "Использует систему для регистрации, для регистрации пациента, поиска пациентов, создания, добавления к пациенту, получения и удаления записей."

        Notifications = softwareSystem "Notification Service" "Сервис отправки уведомлений."
        
        Calendar = softwareSystem "Сalendar Service" "Добавляет, редактирует и удаляет записи в календаре пользователя."
        
        RecordSystem = softwareSystem "Система управлением медицинскими записями" "Система для управления пациентами и их записями." {

            WebUi = container "Web UI" "Веб-интерфейс, для работы с системой управления медицинскими записями." "Web Application"

            ApiGateway = container "API Gateway" "Точка входа запросов. Маршрутизирует их далее." "REST API"

            UserService = container "User Service" "Управление пользователями системы." "C++"

            PatientService = container "Patient Service" "Управляет пациентами: регистрация, поиск пациента." "C++"

            AppointmentService = container "Appointment Service" "Управляет записями: создание и добавление записи, получение записей пациента, удаление записи." "C++"

            Database = container "Database" "Хранит пользователей, пациентов, записи." "PostgreSQL"

            WebUi -> ApiGateway "Вызывает API" "HTTPS/JSON"

            ApiGateway -> UserService "Маршрутизация запросов управления пользователями" "HTTP/JSON"
            ApiGateway -> PatientService "Маршрутизация запросов управления пациентами" "HTTP/JSON"
            ApiGateway -> AppointmentService "Маршрутизация запросов управления записями" "HTTP/JSON"

            UserService -> Database "Создает и ищет и удаляет пользователей" "PostgreSQL"
            PatientService -> Database "Создает и ищет и удаляет пациентов" "PostgreSQL"
            AppointmentService -> Database "Создает и ищет и удаляет записи" "PostgreSQL"
        }

        User -> RecordSystem.webUi "Взаимодействие через веб интерфейс" "HTTPS"
        RecordSystem.apiGateway -> Notifications "Программное взаимодействие" "HTTPS/JSON"
	RecordSystem.apiGateway -> Calendar "Программное взаимодействие" "HTTPS/JSON"
    }

    views {
        themes default

        systemContext RecordSystem "system-context" {
            include *
            autolayout lr
        }

        container RecordSystem "containers" {
            include *
            autolayout lr
        }

        dynamic RecordSystem "create-appointment-for-patient" "Создание записи для пациента" {
            1: User -> RecordSystem.WebUi "Выбирает пациента и создает запись"
            2: RecordSystem.WebUi -> RecordSystem.ApiGateway "POST /patients/{patientId}/appointment"
            3: RecordSystem.ApiGateway -> RecordSystem.PatientService "Получение сведений о пациенте"
            4: RecordSystem.PatientService -> RecordSystem.Database "Поиск по id пациента"
            5: RecordSystem.ApiGateway -> RecordSystem.AppointmentService "Создание записи"
            7: RecordSystem.AppointmentService -> RecordSystem.Database "Вставка записи в базу"
            8: RecordSystem.ApiGateway -> RecordSystem.WebUi "Возвращение результата пользователю"
            9: RecordSystem.ApiGateway -> Notifications "Уведомить о создании записи результата"
            10: RecordSystem.ApiGateway -> Calendar "Cоздание отметки о записи в календаре"
        }

        styles {
            element "Person" {
                shape person
                background #1168bd
                color #ffffff
            }

            element "softwareSystem" {
                background #1168bd
                color #ffffff
            }

            element "Container" {
                background #438dd5
                color #ffffff
            }

            element "Database" {
                shape cylinder
            }
        }
    }
}