import json
import qq_api
from typing import Optional, Union, Dict, List, Any


class QQApi:
    API_OUTLOG = 1 
    API_SEND_MESSAGES = 2
    API_SEND_MESSAGES_ARK = 3
    API_DELETE_MESSAGES = 4
    API_GENERATE_SHARE_LINK = 5
    API_RESPOND_INTERACTION = 6
    API_BOT_LIST = 7
    API_GET_USER_OPENID=8
    API_GET_USER_NAME=9
    API_HTTP=10
    def __init__(self, uuid: str):
        self.uuid = uuid

    def _callback(self, api_id: int, appid: int, *args):
        padded = list(args) + [""] * (8 - len(args))
        padded = [str(x) if x is not None else "" for x in padded]
        result_str = qq_api.Callback(self.uuid, api_id, appid, *padded)
        if result_str:
            try:
                return json.loads(result_str)
            except json.JSONDecodeError:
                return {"error": "Invalid JSON from C++", "raw": result_str}
        return {}

    # ---------- 具体 API 封装 ----------
    def outlog(self, text: str, color_rgb: Optional[int] = None) -> Dict:
        return self._callback(self.API_OUTLOG,0, text, str(color_rgb) if color_rgb is not None else None)
        
    def send_messageEx(self,msg: qq_api.MessageEvent, text: str, is_wakeup: bool = False) -> Dict:
        return self._callback(self.API_SEND_MESSAGES,msg.appid,
                              msg.type, msg.groupid, text,msg.msgid,
                              "true" if is_wakeup else "false", None, None)
                              
    def send_message(self,appid: int, type_: int, openid: str, text: str,msgid: str = "", is_wakeup: bool = False) -> Dict:
        """
        发送普通消息。
        :param type_: 消息类型，0=群聊，1=频道，2=私聊，3=频道私聊
        :param openid: 接收者的 openid
        :param text: 消息内容
        :param message_reference: 引用消息ID（可选）
        :param msgid: 消息ID，空字符串表示主动模式
        :param is_wakeup: 是否为私聊的唤醒消息（与 msgid 互斥，仅私聊有效）
        """
        # API_SEND_MESSAGES: _1=type, _2=openid, _3=text,  _4=msgid, _5=is_wakeup, _7=None, _8=None
        return self._callback(self.API_SEND_MESSAGES,appid,
                              type_, openid, text, msgid,
                              "true" if is_wakeup else "false", None, None)

    def send_ark(self, appid: int,type_: int, openid: str, ark: Union[Dict, str],
                 msgid: str = "", is_wakeup: bool = False) -> Dict:
        """
        发送 ARK 卡片消息。
        :param type_: 消息类型，0=群聊，1=频道，2=私聊，3=频道私聊
        :param openid: 接收者 openid
        :param ark: ARK 数据（字典或 JSON 字符串）
        :param msgid: 消息ID，空字符串表示主动模式
        :param is_wakeup: 是否为私聊的唤醒消息（与 msgid 互斥，仅私聊有效）
        """
        ark_str = ark if isinstance(ark, str) else json.dumps(ark, ensure_ascii=False)
        # API_SEND_MESSAGES_ARK: _1=type, _2=openid, _3=ark, _4=msgid, _5=is_wakeup, _6=None, _7=None, _8=None
        return self._callback(self.API_SEND_MESSAGES_ARK,appid,type_, openid, ark_str, msgid,
                              "true" if is_wakeup else "false")

    def delete_message(self,appid: int, type_: int, openid: str, msgid: str) -> Dict:
        """
        删除消息。
        :param type_: 消息类型，0=群聊，1=频道，2=私聊，3=频道私聊
        :param openid: 会话对象ID
        :param msgid: 要删除的消息ID
        """
        # API_DELETE_MESSAGES: _1=type, _2=openid, _3=msgid
        return self._callback(self.API_DELETE_MESSAGES,appid, type_, openid, msgid)

    def generate_share_link(self,appid:int, callback_data: str) -> Dict:
        """
        生成分享链接。
        :param callback_data: 回调数据
        """
        # API_GENERATE_SHARE_LINK: _1=callback_data
        return self._callback(self.API_GENERATE_SHARE_LINK,appid, callback_data)

    def respond_interaction(self, appid: int,interaction_id: str, code: int, data: str) -> Dict:
        """
        响应交互事件。
        :param interaction_id: 交互ID
        :param code: 响应码（如 0 表示成功）
        :param data: 响应数据（JSON 字符串）
        """
        # API_RESPOND_INTERACTION: _1=interaction_id, _2=code, _3=data
        return self._callback(self.API_RESPOND_INTERACTION,appid,interaction_id, str(code), data)
        
    def botlist(self) -> List[Dict[str, Any]]:
        """
        获取 Bot 列表，每个字典包含以下字段（由 C++ 提供）：
        - appid, name, qq, avatarPath, received, send, online, id, union_openid, startup_time
        - online_duration: 已格式化的在线时长字符串，例如 "2天3小时5分钟"
        """
        raw = self._callback(self.API_BOT_LIST, 0)
        # _callback 正常情况下返回列表（因为 C++ 返回 JSON 数组）
        if isinstance(raw, list):
            return raw
        else:
            # 异常情况返回空列表，也可以打印日志
            return []

    def get_openid(self,appid: int ,user_id:int) -> Dict:
        return self._callback(self.API_GET_USER_OPENID, appid, str(user_id))
        
    def get_user_name(self,appid:int ,user_id:int) -> Dict:
        return self._callback(self.API_GET_USER_NAME, appid, str(user_id))

    def http_request(self, url: str, method: str = "GET", headers: dict = None, body: bytes = None, timeout: int = 30) -> dict:
        headers_json = json.dumps(headers or {})
        body_b64 = base64.b64encode(body).decode('ascii') if body is not None else ""
        return self._callback(self.API_HTTP, 0, url, method.upper(), headers_json, body_b64, str(timeout))
 