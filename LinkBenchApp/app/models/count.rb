class Count < ActiveRecord::Base
	validates :link_type, :version, :time, :countval, presence: true
	belongs_to :node
	validates_presence_of :node
	validates_uniqueness_of :node, :scope => :link_type   # composite uniquness
end
