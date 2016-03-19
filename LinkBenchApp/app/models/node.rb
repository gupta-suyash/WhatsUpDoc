class Node < ActiveRecord::Base
	validates :tnode, :version, :time, :data, presence: true
	has_many :active_friends, class_name: "Link", foreign_key: "node_id", 
			dependent:  :destroy
	has_many :friendis, through: :active_friends, source: :id2
	has_many :is_a_friend, class_name: "Link", foreign_key: "id2", 
			dependent: :destroy
	has_many :friendof, through: :is_a_friend, source: :node_id
	has_one :count, dependent: :destroy
end
