//
// Created by paolo on 05/12/19.
//

#pragma once

constexpr char CLIENT_ID[] = "client_id";
constexpr uint16_t CLIENT_ID_LEN = sizeof(CLIENT_ID)-1;
constexpr char SCOPE[]="scope";
constexpr uint16_t SCOPE_LEN= sizeof(SCOPE)-1;
constexpr char PROFILE[]="https://www.googleapis.com/auth/drive.file%20profile";
constexpr uint16_t PROFILE_LEN=sizeof(PROFILE)-1;
constexpr char CLIENT_SECRET[] ="client_secret";
constexpr uint16_t CLIENT_SECRET_LEN=sizeof(CLIENT_SECRET)-1;
constexpr char CODE[] ="code";
constexpr uint16_t CODE_LEN=sizeof(CODE)-1;
constexpr char GRANT[]="grant_type";
constexpr uint16_t GRANT_LEN=sizeof(GRANT)-1;
constexpr char GRANT_TYPE[]="http://oauth.net/grant_type/device/1.0";
constexpr uint16_t GRANT_TYPE_LEN=sizeof(GRANT_TYPE)-1;
constexpr char CONTENT_LENGTH[] = "Content-Length";
constexpr char CONTENT_TYPE[] = "Content-Type";
constexpr uint16_t CONTENT_TYPE_LEN=sizeof(CONTENT_TYPE)-1;
constexpr char CONTENT_TYPE_FORM[]="application/x-www-form-urlencoded";
constexpr uint16_t CONTENT_TYPE_FORM_LEN=sizeof(CONTENT_TYPE_FORM)-1;
constexpr char MULTIPART_BOUNDARY[]="image_serra_bbb";
constexpr uint16_t MULTIPART_BOUNDARY_LEN= sizeof(MULTIPART_BOUNDARY)-1;
constexpr char CONTENT_TYPE_MULTIPART[]="multipart/related; boundary=image_serra_bbb";
constexpr char CONTENT_JSON[] = " application/json; charset=UTF-8";
constexpr uint16_t CONTENT_JSON_LEN = sizeof(CONTENT_JSON)-1;
constexpr char REFRESH_TOKEN[] = "refresh_token";
constexpr uint16_t REFRESH_TOKEN_LEN = sizeof(REFRESH_TOKEN)-1;
constexpr char AUTHORIZATION_HEADER[] = "Authorization";
constexpr uint16_t AUTHORIZATION_HEADER_LEN = sizeof(AUTHORIZATION_HEADER)-1;;
constexpr char BEARER_HEADER[] = "Bearer ";
constexpr uint16_t BEARER_HEADER_LEN = sizeof(BEARER_HEADER)-1;;
constexpr char FILTER_FOR_FOLDER[]="q";
constexpr char FILTER_FOR_FOLDER_LEN= sizeof(FILTER_FOR_FOLDER)-1;
constexpr char FILTER_FOR_FOLDER_VALUE[]= "mimeType = 'application/vnd.google-apps.folder' and name='serra'";
constexpr char FILTER_FOR_FOLDER_VALUE_LEN= sizeof(FILTER_FOR_FOLDER_VALUE)-1;
constexpr char UPDATE_FILE_URL[] = "https://www.googleapis.com/upload/drive/v3/files/";
constexpr char UPDATE_FILE_URL_LEN= sizeof(UPDATE_FILE_URL)-1;
constexpr char UPLOAD_TYPE_MEDIA[] = "?uploadType=media";
constexpr char UPLOAD_TYPE_MEDIA_LEN= sizeof(UPLOAD_TYPE_MEDIA)-1;